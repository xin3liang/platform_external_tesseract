/* -*-C-*-
 ********************************************************************************
 *
 * File:        dawg.c  (Formerly dawg.c)
 * Description:  Use a Directed Accyclic Word Graph
 * Author:       Mark Seaman, OCR Technology
 * Created:      Fri Oct 16 14:37:00 1987
 * Modified:     Wed Jul 24 16:59:16 1991 (Mark Seaman) marks@hpgrlt
 * Language:     C
 * Package:      N/A
 * Status:       Reusable Software Component
 *
 * (c) Copyright 1987, Hewlett-Packard Company.
 ** Licensed under the Apache License, Version 2.0 (the "License");
 ** you may not use this file except in compliance with the License.
 ** You may obtain a copy of the License at
 ** http://www.apache.org/licenses/LICENSE-2.0
 ** Unless required by applicable law or agreed to in writing, software
 ** distributed under the License is distributed on an "AS IS" BASIS,
 ** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 ** See the License for the specific language governing permissions and
 ** limitations under the License.
 *
 *********************************************************************************/
/*----------------------------------------------------------------------
              I n c l u d e s
----------------------------------------------------------------------*/
#include "dawg.h"

#include "context.h"
#include "cutil.h"
#include "dict.h"
#include "emalloc.h"
#include "freelist.h"
#include "helpers.h"
#include "strngs.h"
#include "tprintf.h"
#include "math.h"

/*----------------------------------------------------------------------
              V a r i a b l e s
----------------------------------------------------------------------*/
INT_VAR(dawg_debug_level, 0, "Set to 1 for general debug info"
        ", to 2 for more details, to 3 to see all the debug messages");

/*----------------------------------------------------------------------
              F u n c t i o n s   f o r   D a w g
----------------------------------------------------------------------*/
namespace tesseract {

bool Dawg::word_in_dawg(const WERD_CHOICE &word) const {
  if (word.length() == 0) return false;
  NODE_REF node = 0;
  int end_index = word.length() - 1;
  for (int i = 0; i <= end_index; i++) {
    if (dawg_debug_level > 1) {
      tprintf("word_in_dawg: exploring node " REFFORMAT ":\n", node);
      print_node(node, MAX_NODE_EDGES_DISPLAY);
      tprintf("\n");
  }
    EDGE_REF edge = edge_char_of(node, word.unichar_id(i), i == end_index);
    if (edge != NO_EDGE) {
      node = next_node(edge);
      if (node == 0) node = NO_EDGE;
    } else {
      return false;
}
    }
  return true;
    }

int Dawg::check_for_words(const char *filename,
                          const UNICHARSET &unicharset,
                          bool enable_wildcard) const {
  if (filename == NULL) return 0;

  FILE       *word_file;
  char       string [CHARS_PER_LINE];
  int misses = 0;
  UNICHAR_ID wildcard = unicharset.unichar_to_id(kWildcard);

  word_file = open_file (filename, "r");

  while (fgets (string, CHARS_PER_LINE, word_file) != NULL) {
    chomp_string(string);  // remove newline
    WERD_CHOICE word(string, unicharset);
    if (word.length() > 0 &&
        !word.contains_unichar_id(INVALID_UNICHAR_ID)) {
      if (!match_words(&word, 0, 0,
                       enable_wildcard ? wildcard : INVALID_UNICHAR_ID)) {
        tprintf("Missing word: %s\n", string);
        ++misses;
  }
    } else {
      tprintf("Failed to create a valid word from %s\n", string);
  }
}
  fclose (word_file);
  // Make sure the user sees this with fprintf instead of tprintf.
  if (dawg_debug_level) tprintf("Number of lost words=%d\n", misses);
  return misses;
}

bool Dawg::match_words(WERD_CHOICE *word, inT32 index,
                       NODE_REF node, UNICHAR_ID wildcard) const {
  EDGE_REF     edge;
  inT32 word_end;

  if (wildcard != INVALID_UNICHAR_ID && word->unichar_id(index) == wildcard) {
    bool any_matched = false;
    NodeChildVector vec;
    this->unichar_ids_of(node, &vec);
    for (int i = 0; i < vec.size(); ++i) {
      word->set_unichar_id(vec[i].unichar_id, index);
      if (match_words(word, index, node, wildcard))
        any_matched = true;
    }
    word->set_unichar_id(wildcard, index);
    return any_matched;
    } else {
    word_end = index == word->length() - 1;
    edge = edge_char_of(node, word->unichar_id(index), word_end);
    if (edge != NO_EDGE) {  // normal edge in DAWG
      node = next_node(edge);
      if (word_end) {
        if (dawg_debug_level > 1) word->print("match_words() found: ");
        return true;
      } else if (node != 0) {
        return match_words(word, index+1, node, wildcard);
    }
  }
    }
  return false;
}

void Dawg::init(DawgType type, const STRING &lang,
                PermuterType perm, int unicharset_size) {
  type_ = type;
  lang_ = lang;
  perm_ = perm;
  ASSERT_HOST(unicharset_size > 0);
  unicharset_size_ = unicharset_size;
  // Set bit masks.
#ifdef WIN32
  flag_start_bit_ = ceil(log(static_cast<double>(unicharset_size_))/log(2.0));
#else
  flag_start_bit_ = ceil(log(unicharset_size_)/log(2.0));
#endif
  next_node_start_bit_ = flag_start_bit_ + NUM_FLAG_BITS;
  letter_mask_ = ~(~0 << flag_start_bit_);
  next_node_mask_ = ~0 << (flag_start_bit_ + NUM_FLAG_BITS);
  flags_mask_ = ~(letter_mask_ | next_node_mask_);
    }


/*----------------------------------------------------------------------
         F u n c t i o n s   f o r   S q u i s h e d    D a w g
----------------------------------------------------------------------*/

SquishedDawg::~SquishedDawg() { memfree(edges_); }

EDGE_REF SquishedDawg::edge_char_of(NODE_REF node,
                                    UNICHAR_ID unichar_id,
                                    bool word_end) const {
  EDGE_REF edge = node;
  if (node == 0) {  // binary search
    EDGE_REF start = 0;
    EDGE_REF end = num_forward_edges_in_node0 - 1;
    int compare;
    while (start <= end) {
      edge = (start + end) >> 1;  // (start + end) / 2
      compare = given_greater_than_edge_rec(NO_EDGE, word_end,
                                            unichar_id, edges_[edge]);
      if (compare == 0) {  // given == vec[k]
        return edge;
      } else if (compare == 1) {  // given > vec[k]
        start = edge + 1;
      } else {  // given < vec[k]
        end = edge - 1;
  }
}
  } else {  // linear search
    if (edge != NO_EDGE && edge_occupied(edge)) {
      do {
        if ((unichar_id_from_edge_rec(edges_[edge]) == unichar_id) &&
            (!word_end || end_of_word_from_edge_rec(edges_[edge])))
          return (edge);
      } while (!last_edge(edge++));
    }
  }
  return (NO_EDGE);  // not found
}

inT32 SquishedDawg::num_forward_edges(NODE_REF node) const {
  EDGE_REF   edge = node;
  inT32        num  = 0;

  if (forward_edge (edge)) {
    do {
      num++;
    } while (!last_edge(edge++));
  }

  return (num);
}

void SquishedDawg::print_node(NODE_REF node, int max_num_edges) const {
  if (node == NO_EDGE) return;  // nothing to print

  EDGE_REF   edge = node;
  const char       *forward_string  = "FORWARD";
  const char       *backward_string = "       ";

  const char       *last_string     = "LAST";
  const char       *not_last_string = "    ";

  const char       *eow_string      = "EOW";
  const char       *not_eow_string  = "   ";

  const char       *direction;
  const char       *is_last;
  const char       *eow;

  UNICHAR_ID unichar_id;

  if (edge_occupied(edge)) {
    do {
      direction =
        forward_edge(edge) ? forward_string : backward_string;
      is_last = last_edge(edge) ? last_string : not_last_string;
      eow = end_of_word(edge) ? eow_string : not_eow_string;

      unichar_id = edge_letter(edge);
      tprintf(REFFORMAT " : next = " REFFORMAT ", unichar_id = %d, %s %s %s\n",
              edge, next_node(edge), unichar_id,
        direction, is_last, eow);

      if (edge - node > max_num_edges) return;
    } while (!last_edge(edge++));

    if (edge < num_edges_ &&
        edge_occupied(edge) && backward_edge(edge)) {
      do {
        direction =
          forward_edge(edge) ? forward_string : backward_string;
        is_last = last_edge(edge) ? last_string : not_last_string;
        eow = end_of_word(edge) ? eow_string : not_eow_string;

        unichar_id = edge_letter(edge);
        tprintf(REFFORMAT " : next = " REFFORMAT
                ", unichar_id = %d, %s %s %s\n",
                edge, next_node(edge), unichar_id,
          direction, is_last, eow);

        if (edge - node > MAX_NODE_EDGES_DISPLAY) return;
      } while (!last_edge(edge++));
    }
  }
  else {
    tprintf(REFFORMAT " : no edges in this node\n", node);
  }
  tprintf("\n");
}

void SquishedDawg::print_edge(EDGE_REF edge) const {
  if (edge == NO_EDGE) {
    tprintf("NO_EDGE\n");
  } else {
    tprintf(REFFORMAT " : next = " REFFORMAT
            ", unichar_id = '%d', %s %s %s\n", edge,
            next_node(edge), edge_letter(edge),
            (forward_edge(edge) ? "FORWARD" : "       "),
            (last_edge(edge) ? "LAST"    : "    "),
            (end_of_word(edge) ? "EOW"     : ""));
  }
}

void SquishedDawg::read_squished_dawg(FILE *file, DawgType type,
                                      const STRING &lang, PermuterType perm) {
  if (dawg_debug_level) tprintf("Reading squished dawg\n");

  // Read the magic number and if it does not match kDawgMagicNumber
  // set swap to true to indicate that we need to switch endianness.
  inT16 magic;
  fread(&magic, sizeof(inT16), 1, file);
  bool swap = (magic != kDawgMagicNumber);

  int unicharset_size;
  fread(&unicharset_size, sizeof(inT32), 1, file);
  fread(&num_edges_, sizeof(inT32), 1, file);

  if (swap) {
    unicharset_size = reverse32(unicharset_size);
    num_edges_ = reverse32(num_edges_);
  }
  Dawg::init(type, lang, perm, unicharset_size);

  edges_ = (EDGE_ARRAY) memalloc(sizeof(EDGE_RECORD) * num_edges_);
  fread(&edges_[0], sizeof(EDGE_RECORD), num_edges_, file);
  EDGE_REF edge;
  if (swap) {
    for (edge = 0; edge < num_edges_; ++edge) {
      edges_[edge] = reverse64(edges_[edge]);
    }
  }
  if (dawg_debug_level > 2) {
    tprintf("type: %d lang: %s perm: %d unicharset_size: %d num_edges: %d\n",
            type_, lang_.string(), perm_, unicharset_size_, num_edges_);
    for (edge = 0; edge < num_edges_; ++edge)
      print_edge(edge);
  }
}

NODE_MAP SquishedDawg::build_node_map(inT32 *num_nodes) const {
  EDGE_REF   edge;
  NODE_MAP   node_map;
  inT32       node_counter;
  inT32       num_edges;

  node_map = (NODE_MAP) malloc(sizeof(EDGE_REF) * num_edges_);

  for (edge=0; edge < num_edges_; edge++)       // init all slots
    node_map [edge] = -1;

  node_counter = num_forward_edges(0);

  *num_nodes   = 0;
  for (edge=0; edge < num_edges_; edge++) {     // search all slots

    if (forward_edge(edge)) {
      (*num_nodes)++;                          // count nodes links
      node_map[edge] = (edge ? node_counter : 0);
      num_edges = num_forward_edges(edge);
      if (edge != 0) node_counter += num_edges;
      edge += num_edges;
      if (backward_edge(edge)) while (!last_edge(edge++));
      edge--;
    }
  }
  return (node_map);
}

void SquishedDawg::write_squished_dawg(const char *filename) {
  FILE       *file;
  EDGE_REF    edge;
  inT32       num_edges;
  inT32       node_count = 0;
  NODE_MAP    node_map;
  EDGE_REF    old_index;
  EDGE_RECORD temp_record;

  if (dawg_debug_level) tprintf("write_squished_dawg\n");

  node_map = build_node_map(&node_count);

#ifdef WIN32
  file = open_file(filename, "wb");
#else
  file = open_file(filename, "w");
#endif

  // Write the magic number to help detecting a change in endianness.
  inT16 magic = kDawgMagicNumber;
  fwrite(&magic, sizeof(inT16), 1, file);
  fwrite(&unicharset_size_, sizeof(inT32), 1, file);

  // Count the number of edges in this Dawg.
  num_edges = 0;
  for (edge=0; edge < num_edges_; edge++)
    if (forward_edge(edge))
      num_edges++;

  fwrite(&num_edges, sizeof(inT32), 1, file);  // write edge count to file

  if (dawg_debug_level) {
    tprintf("%d nodes in DAWG\n", node_count);
    tprintf("%d edges in DAWG\n", num_edges);
}

  for (edge=0; edge<num_edges_; edge++) {
    if (forward_edge(edge)) {  // write forward edges
      do {
        old_index = next_node_from_edge_rec(edges_[edge]);
        set_next_node(edge, node_map[old_index]);
        temp_record = edges_[edge];
        fwrite(&(temp_record), sizeof(EDGE_RECORD), 1, file);
        set_next_node(edge, old_index);
      } while (!last_edge(edge++));

      if (backward_edge(edge))  // skip back links
        while (!last_edge(edge++));

      edge--;
    }
    }
  free(node_map);
  fclose(file);
  }

}  // namespace tesseract
