/******************************************************************
 * File:        fixspace.cpp  (Formerly fixspace.c)
 * Description: Implements a pass over the page res, exploring the alternative
 *					spacing possibilities, trying to use context to improve the
          word spacing
* Author:		Phil Cheatle
* Created:		Thu Oct 21 11:38:43 BST 1993
*
* (C) Copyright 1993, Hewlett-Packard Ltd.
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
**********************************************************************/

#include "mfcpch.h"
#include <ctype.h>
#include "reject.h"
#include "statistc.h"
#include "genblob.h"
#include "control.h"
#include "fixspace.h"
#include "tessvars.h"
#include "tessbox.h"
#include "secname.h"
#include "globals.h"
#include "tesseractclass.h"

#define EXTERN

EXTERN BOOL_VAR (fixsp_check_for_fp_noise_space, TRUE,
"Try turning noise to space in fixed pitch");
EXTERN BOOL_VAR (fixsp_fp_eval, TRUE, "Use alternate evaluation for fp");
EXTERN BOOL_VAR (fixsp_noise_score_fixing, TRUE, "More sophisticated?");
EXTERN INT_VAR (fixsp_non_noise_limit, 1,
"How many non-noise blbs either side?");
EXTERN double_VAR (fixsp_small_outlines_size, 0.28, "Small if lt xht x this");

EXTERN BOOL_VAR (fixsp_ignore_punct, TRUE, "In uniform spacing calc");
EXTERN BOOL_VAR (fixsp_numeric_fix, TRUE, "Try to deal with numeric punct");
EXTERN BOOL_VAR (fixsp_prefer_joined_1s, TRUE, "Arbitrary boost");
EXTERN BOOL_VAR (tessedit_test_uniform_wd_spacing, FALSE,
"Limit context word spacing");
EXTERN BOOL_VAR (tessedit_prefer_joined_punct, FALSE,
"Reward punctation joins");
EXTERN INT_VAR (fixsp_done_mode, 1, "What constitues done for spacing");
EXTERN INT_VAR (debug_fix_space_level, 0, "Contextual fixspace debug");
EXTERN STRING_VAR (numeric_punctuation, ".,",
"Punct. chs expected WITHIN numbers");

#define PERFECT_WERDS   999
#define MAXSPACING      128      /*max expected spacing in pix */

/*************************************************************************
 * fix_fuzzy_spaces()
 * Walk over the page finding sequences of words joined by fuzzy spaces. Extract
 * them as a sublist, process the sublist to find the optimal arrangement of
 * spaces then replace the sublist in the ROW_RES.
 *************************************************************************/
namespace tesseract {
void Tesseract::fix_fuzzy_spaces(                       //find fuzzy words
                                                        //progress monitor
                                 volatile ETEXT_DESC *monitor,
                                                        //count of words in doc
                                 inT32 word_count,
                                 PAGE_RES *page_res) {
  BLOCK_RES_IT block_res_it;     //iterators
  ROW_RES_IT row_res_it;
  WERD_RES_IT word_res_it_from;
  WERD_RES_IT word_res_it_to;
  WERD_RES *word_res;
  WERD_RES_LIST fuzzy_space_words;
  inT16 new_length;
  BOOL8 prevent_null_wd_fixsp;   //DONT process blobless wds
  inT32 word_index;              //current word

  block_res_it.set_to_list (&page_res->block_res_list);
  word_index = 0;
  for (block_res_it.mark_cycle_pt ();
  !block_res_it.cycled_list (); block_res_it.forward ()) {
    row_res_it.set_to_list (&block_res_it.data ()->row_res_list);
    for (row_res_it.mark_cycle_pt ();
    !row_res_it.cycled_list (); row_res_it.forward ()) {
      word_res_it_from.set_to_list (&row_res_it.data ()->word_res_list);
      while (!word_res_it_from.at_last ()) {
        word_res = word_res_it_from.data ();
        while (!word_res_it_from.at_last () &&
          !(word_res->combination ||
          word_res_it_from.data_relative (1)->
          word->flag (W_FUZZY_NON) ||
          word_res_it_from.data_relative (1)->
        word->flag (W_FUZZY_SP))) {
          fix_sp_fp_word(word_res_it_from, row_res_it.data()->row,
                         block_res_it.data()->block);
          word_res = word_res_it_from.forward ();
          word_index++;
          if (monitor != NULL) {
            monitor->ocr_alive = TRUE;
            monitor->progress = 90 + 5 * word_index / word_count;
          }
        }

        if (!word_res_it_from.at_last ()) {
          word_res_it_to = word_res_it_from;
          prevent_null_wd_fixsp =
            word_res->word->gblob_list ()->empty ();
          if (check_debug_pt (word_res, 60))
            debug_fix_space_level.set_value (10);
          word_res_it_to.forward ();
          word_index++;
          if (monitor != NULL) {
            monitor->ocr_alive = TRUE;
            monitor->progress = 90 + 5 * word_index / word_count;
          }
          while (!word_res_it_to.at_last () &&
            (word_res_it_to.data_relative (1)->
            word->flag (W_FUZZY_NON) ||
            word_res_it_to.data_relative (1)->
          word->flag (W_FUZZY_SP))) {
            if (check_debug_pt (word_res, 60))
              debug_fix_space_level.set_value (10);
            if (word_res->word->gblob_list ()->empty ())
              prevent_null_wd_fixsp = TRUE;
            word_res = word_res_it_to.forward ();
          }
          if (check_debug_pt (word_res, 60))
            debug_fix_space_level.set_value (10);
          if (word_res->word->gblob_list ()->empty ())
            prevent_null_wd_fixsp = TRUE;
          if (prevent_null_wd_fixsp) {
            word_res_it_from = word_res_it_to;
          } else {
            fuzzy_space_words.assign_to_sublist (&word_res_it_from,
              &word_res_it_to);
            fix_fuzzy_space_list (fuzzy_space_words,
                                 row_res_it.data()->row,
                                 block_res_it.data()->block);
            new_length = fuzzy_space_words.length ();
            word_res_it_from.add_list_before (&fuzzy_space_words);
            for (;
              (!word_res_it_from.at_last () &&
            (new_length > 0)); new_length--) {
              word_res_it_from.forward ();
            }
          }
          if (test_pt)
            debug_fix_space_level.set_value (0);
        }
        fix_sp_fp_word(word_res_it_from, row_res_it.data ()->row,
                       block_res_it.data()->block);
        //Last word in row
      }
    }
  }
}

void Tesseract::fix_fuzzy_space_list(  //space explorer
                                     WERD_RES_LIST &best_perm,
                                     ROW *row,
                                     BLOCK* block) {
  inT16 best_score;
  WERD_RES_LIST current_perm;
  inT16 current_score;
  BOOL8 improved = FALSE;

  best_score = eval_word_spacing(best_perm);  // default score
  dump_words (best_perm, best_score, 1, improved);

  if (best_score != PERFECT_WERDS)
    initialise_search(best_perm, current_perm);

  while ((best_score != PERFECT_WERDS) && !current_perm.empty ()) {
    match_current_words(current_perm, row, block);
    current_score = eval_word_spacing (current_perm);
    dump_words (current_perm, current_score, 2, improved);
    if (current_score > best_score) {
      best_perm.clear ();
      best_perm.deep_copy(&current_perm, &WERD_RES::deep_copy);
      best_score = current_score;
      improved = TRUE;
    }
    if (current_score < PERFECT_WERDS)
      transform_to_next_perm(current_perm);
  }
  dump_words (best_perm, best_score, 3, improved);
}

}  // namespace tesseract

void initialise_search(WERD_RES_LIST &src_list, WERD_RES_LIST &new_list) {
  WERD_RES_IT src_it(&src_list);
  WERD_RES_IT new_it(&new_list);
  WERD_RES *src_wd;
  WERD_RES *new_wd;

  for (src_it.mark_cycle_pt (); !src_it.cycled_list (); src_it.forward ()) {
    src_wd = src_it.data ();
    if (!src_wd->combination) {
      new_wd = new WERD_RES (*src_wd);
      new_wd->combination = FALSE;
      new_wd->part_of_combo = FALSE;
      new_it.add_after_then_move (new_wd);
    }
  }
}


namespace tesseract {
void Tesseract::match_current_words(WERD_RES_LIST &words, ROW *row,
                                    BLOCK* block) {
  WERD_RES_IT word_it(&words);
  WERD_RES *word;

  for (word_it.mark_cycle_pt (); !word_it.cycled_list (); word_it.forward ()) {
    word = word_it.data ();
    if ((!word->part_of_combo) && (word->outword == NULL))
      classify_word_pass2(word, block, row);
  }
}


/*************************************************************************
 * eval_word_spacing()
 * The basic measure is the number of characters in contextually confirmed
 * words. (I.e the word is done)
 * If all words are contextually confirmed the evaluation is deemed perfect.
 *
 * Some fiddles are done to handle "1"s as these are VERY frequent causes of
 * fuzzy spaces. The problem with the basic measure is that "561 63" would score
 * the same as "56163", though given our knowledge that the space is fuzzy, and
 * that there is a "1" next to the fuzzy space, we need to ensure that "56163"
 * is prefered.
 *
 * The solution is to NOT COUNT the score of any word which has a digit at one
 * end and a "1Il" as the character the other side of the space.
 *
 * Conversly, any character next to a "1" within a word is counted as a positive
 * score. Thus "561 63" would score 4 (3 chars in a numeric word plus 1 side of
 * the "1" joined).  "56163" would score 7 - all chars in a numeric word + 2
 * sides of a "1" joined.
 *
 * The joined 1 rule is applied to any word REGARDLESS of contextual
 * confirmation.  Thus "PS7a71 3/7a" scores 1 (neither word is contexutally
 * confirmed. The only score is from the joined 1. "PS7a713/7a" scores 2.
 *
 *************************************************************************/
inT16 Tesseract::eval_word_spacing(WERD_RES_LIST &word_res_list) {
  WERD_RES_IT word_res_it(&word_res_list);
  inT16 total_score = 0;
  inT16 word_count = 0;
  inT16 done_word_count = 0;
  inT16 word_len;
  inT16 i;
  inT16 offset;
  WERD_RES *word;                //current word
  inT16 prev_word_score = 0;
  BOOL8 prev_word_done = FALSE;
  BOOL8 prev_char_1 = FALSE;     //prev ch a "1/I/l"?
  BOOL8 prev_char_digit = FALSE; //prev ch 2..9 or 0
  BOOL8 current_char_1 = FALSE;
  BOOL8 current_word_ok_so_far;
  STRING punct_chars = "!\"`',.:;";
  BOOL8 prev_char_punct = FALSE;
  BOOL8 current_char_punct = FALSE;
  BOOL8 word_done = FALSE;

  do {
    word = word_res_it.data ();
    word_done = fixspace_thinks_word_done (word);
    word_count++;
    if (word->tess_failed) {
      total_score += prev_word_score;
      if (prev_word_done)
        done_word_count++;
      prev_word_score = 0;
      prev_char_1 = FALSE;
      prev_char_digit = FALSE;
      prev_word_done = FALSE;
    }
    else {
      /*
        Can we add the prev word score and potentially count this word?
        Yes IF it didnt end in a 1 when the first char of this word is a digit
          AND it didnt end in a digit when the first char of this word is a 1
      */
      word_len = word->reject_map.length ();
      current_word_ok_so_far = FALSE;
      if (!((prev_char_1 &&
        digit_or_numeric_punct (word, 0)) ||
        (prev_char_digit &&
        ((word_done &&
        (word->best_choice->unichar_lengths().string()[0] == 1 &&
         word->best_choice->unichar_string()[0] == '1')) ||
        (!word_done &&
         STRING(conflict_set_I_l_1).contains(
             word->best_choice->unichar_string ()[0])))))) {
        total_score += prev_word_score;
        if (prev_word_done)
          done_word_count++;
        current_word_ok_so_far = word_done;
      }

      if ((current_word_ok_so_far) &&
        (!tessedit_test_uniform_wd_spacing ||
        ((word->best_choice->permuter () == NUMBER_PERM) ||
      uniformly_spaced (word)))) {
        prev_word_done = TRUE;
        prev_word_score = word_len;
      }
      else {
        prev_word_done = FALSE;
        prev_word_score = 0;
      }

      if (fixsp_prefer_joined_1s) {
        /* Add 1 to total score for every joined 1 regardless of context and
           rejtn */

        for (i = 0, prev_char_1 = FALSE; i < word_len; i++) {
          current_char_1 = word->best_choice->unichar_string()[i] == '1';
          if (prev_char_1 || (current_char_1 && (i > 0)))
            total_score++;
          prev_char_1 = current_char_1;
        }
      }

      /* Add 1 to total score for every joined punctuation regardless of context
        and rejtn */
      if (tessedit_prefer_joined_punct) {
        for (i = 0, offset = 0, prev_char_punct = FALSE; i < word_len;
             offset += word->best_choice->unichar_lengths()[i++]) {
          current_char_punct =
            punct_chars.contains (word->best_choice->unichar_string()[offset]);
          if (prev_char_punct || (current_char_punct && (i > 0)))
            total_score++;
          prev_char_punct = current_char_punct;
        }
      }
      prev_char_digit = digit_or_numeric_punct (word, word_len - 1);
      for (i = 0, offset = 0; i < word_len - 1;
           offset += word->best_choice->unichar_lengths()[i++]);
      prev_char_1 =
        ((word_done
        && (word->best_choice->unichar_string()[offset] == '1'))
        || (!word_done
        && STRING(conflict_set_I_l_1).contains(
            word->best_choice->unichar_string()[offset])));
    }
    /* Find next word */
    do
    word_res_it.forward ();
    while (word_res_it.data ()->part_of_combo);
  }
  while (!word_res_it.at_first ());
  total_score += prev_word_score;
  if (prev_word_done)
    done_word_count++;
  if (done_word_count == word_count)
    return PERFECT_WERDS;
  else
    return total_score;
}


BOOL8 Tesseract::digit_or_numeric_punct(WERD_RES *word, int char_position) {
  int i;
  int offset;

  for (i = 0, offset = 0; i < char_position;
       offset += word->best_choice->unichar_lengths()[i++]);
  return (unicharset.get_isdigit(word->best_choice->unichar_string().string() + offset,
                                 word->best_choice->unichar_lengths()[i]) ||
    (fixsp_numeric_fix &&
    (word->best_choice->permuter () == NUMBER_PERM) &&
    STRING (numeric_punctuation).contains
     (word->best_choice->unichar_string().string()[offset])));
}
}  // namespace tesseract


/*************************************************************************
 * transform_to_next_perm()
 * Examines the current word list to find the smallest word gap size. Then walks
 * the word list closing any gaps of this size by either inserted new
 * combination words, or extending existing ones.
 *
 * The routine COULD be limited to stop it building words longer than N blobs.
 *
 * If there are no more gaps then it DELETES the entire list and returns the
 * empty list to cause termination.
 *************************************************************************/
void transform_to_next_perm(WERD_RES_LIST &words) {
  WERD_RES_IT word_it(&words);
  WERD_RES_IT prev_word_it(&words);
  WERD_RES *word;
  WERD_RES *prev_word;
  WERD_RES *combo;
  WERD *copy_word;
  inT16 prev_right = -1;
  TBOX box;
  inT16 gap;
  inT16 min_gap = MAX_INT16;

  for (word_it.mark_cycle_pt (); !word_it.cycled_list (); word_it.forward ()) {
    word = word_it.data ();
    if (!word->part_of_combo) {
      box = word->word->bounding_box ();
      if (prev_right >= 0) {
        gap = box.left () - prev_right;
        if (gap < min_gap)
          min_gap = gap;
      }
      prev_right = box.right ();
    }
  }
  if (min_gap < MAX_INT16) {
    prev_right = -1;             //back to start
    word_it.set_to_list (&words);
    for (;                       //cant use cycle pt due to inserted combos at start of list
    (prev_right < 0) || !word_it.at_first (); word_it.forward ()) {
      word = word_it.data ();
      if (!word->part_of_combo) {
        box = word->word->bounding_box ();
        if (prev_right >= 0) {
          gap = box.left () - prev_right;
          if (gap <= min_gap) {
            prev_word = prev_word_it.data ();
            if (prev_word->combination)
              combo = prev_word;
            else {
              /* Make a new combination and insert before the first word being joined */
              copy_word = new WERD;
              *copy_word = *(prev_word->word);
              //deep copy
              combo = new WERD_RES (copy_word);
              combo->combination = TRUE;
              combo->x_height = prev_word->x_height;
              prev_word->part_of_combo = TRUE;
              prev_word_it.add_before_then_move (combo);
            }
            combo->word->set_flag (W_EOL, word->word->flag (W_EOL));
            if (word->combination) {
              combo->word->join_on (word->word);
              //Move blbs to combo
                                 //old combo no longer needed
              delete word_it.extract ();
            }
            else {
                                 //Cpy current wd to combo
              combo->copy_on (word);
              word->part_of_combo = TRUE;
            }
            combo->done = FALSE;
            if (combo->outword != NULL) {
              delete combo->outword;
              delete combo->best_choice;
              delete combo->raw_choice;
              combo->outword = NULL;
              combo->best_choice = NULL;
              combo->raw_choice = NULL;
            }
          }
          else
                                 //catch up
              prev_word_it = word_it;
        }
        prev_right = box.right ();
      }
    }
  }
  else
    words.clear ();              //signal termination
}


void dump_words(WERD_RES_LIST &perm, inT16 score, inT16 mode, BOOL8 improved) {
  WERD_RES_IT word_res_it(&perm);
  static STRING initial_str;

  if (debug_fix_space_level > 0) {
    if (mode == 1) {
      initial_str = "";
      for (word_res_it.mark_cycle_pt ();
      !word_res_it.cycled_list (); word_res_it.forward ()) {
        if (!word_res_it.data ()->part_of_combo) {
          initial_str += word_res_it.data()->best_choice->unichar_string();
          initial_str += ' ';
        }
      }
    }

    #ifndef SECURE_NAMES
    if (debug_fix_space_level > 1) {
      switch (mode) {
        case 1:
          tprintf ("EXTRACTED (%d): \"", score);
          break;
        case 2:
          tprintf ("TESTED (%d): \"", score);
          break;
        case 3:
          tprintf ("RETURNED (%d): \"", score);
          break;
      }

      for (word_res_it.mark_cycle_pt ();
      !word_res_it.cycled_list (); word_res_it.forward ()) {
        if (!word_res_it.data ()->part_of_combo)
          tprintf("%s/%1d ",
                  word_res_it.data()->best_choice->unichar_string().string(),
                  (int)word_res_it.data()->best_choice->permuter());
      }
      tprintf ("\"\n");
    }
    else if (improved) {
      tprintf ("FIX SPACING \"%s\" => \"", initial_str.string ());
      for (word_res_it.mark_cycle_pt ();
      !word_res_it.cycled_list (); word_res_it.forward ()) {
        if (!word_res_it.data ()->part_of_combo)
          tprintf ("%s/%1d ",
                   word_res_it.data()->best_choice->unichar_string().string(),
                   (int)word_res_it.data()->best_choice->permuter());
      }
      tprintf ("\"\n");
    }
    #endif
  }
}


/*************************************************************************
 * uniformly_spaced()
 * Return true if one of the following are true:
 *    - All inter-char gaps are the same width
 *	- The largest gap is no larger than twice the mean/median of the others
 *	- The largest gap is < 64/5 = 13 and all others are <= 0
 * **** REMEMBER - WE'RE NOW WORKING WITH A BLN WERD !!!
 *************************************************************************/
BOOL8 uniformly_spaced(  //sensible word
                       WERD_RES *word) {
  PBLOB_IT blob_it;
  TBOX box;
  inT16 prev_right = -MAX_INT16;
  inT16 gap;
  inT16 max_gap = -MAX_INT16;
  inT16 max_gap_count = 0;
  STATS gap_stats (0, MAXSPACING);
  BOOL8 result;
  const ROW *row = word->denorm.row ();
  float max_non_space;
  float normalised_max_nonspace;
  inT16 i = 0;
  inT16 offset = 0;
  STRING punct_chars = "\"`',.:;";

  blob_it.set_to_list (word->outword->blob_list ());

  for (blob_it.mark_cycle_pt (); !blob_it.cycled_list (); blob_it.forward ()) {
    box = blob_it.data ()->bounding_box ();
    if ((prev_right > -MAX_INT16) &&
      (!fixsp_ignore_punct ||
      (!punct_chars.contains (word->best_choice->unichar_string()
                              [offset - word->best_choice->unichar_lengths()[i - 1]]) &&
    !punct_chars.contains (word->best_choice->unichar_string()[offset])))) {
      gap = box.left () - prev_right;
      if (gap < max_gap)
        gap_stats.add (gap, 1);
      else if (gap == max_gap)
        max_gap_count++;
      else {
        if (max_gap_count > 0)
          gap_stats.add (max_gap, max_gap_count);
        max_gap = gap;
        max_gap_count = 1;
      }
    }
    prev_right = box.right ();
    offset += word->best_choice->unichar_lengths()[i++];
  }

  max_non_space = (row->space () + 3 * row->kern ()) / 4;
  normalised_max_nonspace = max_non_space * bln_x_height / row->x_height ();

  result = ((gap_stats.get_total () == 0) ||
    (max_gap <= normalised_max_nonspace) ||
    ((gap_stats.get_total () > 2) &&
    (max_gap <= 2 * gap_stats.median ())) ||
    ((gap_stats.get_total () <= 2) &&
    (max_gap <= 2 * gap_stats.mean ())));
  #ifndef SECURE_NAMES
  if ((debug_fix_space_level > 1)) {
    if (result)
      tprintf
        ("ACCEPT SPACING FOR: \"%s\" norm_maxnon = %f max=%d maxcount=%d total=%d mean=%f median=%f\n",
        word->best_choice->unichar_string().string (), normalised_max_nonspace,
        max_gap, max_gap_count, gap_stats.get_total (), gap_stats.mean (),
        gap_stats.median ());
    else
      tprintf
        ("REJECT SPACING FOR: \"%s\" norm_maxnon = %f max=%d maxcount=%d total=%d mean=%f median=%f\n",
        word->best_choice->unichar_string().string (), normalised_max_nonspace,
        max_gap, max_gap_count, gap_stats.get_total (), gap_stats.mean (),
        gap_stats.median ());
  }
  #endif

  return result;
}


BOOL8 fixspace_thinks_word_done(WERD_RES *word) {
  if (word->done)
    return TRUE;

  /*
    Use all the standard pass 2 conditions for mode 5 in set_done() in
    reject.c BUT DONT REJECT IF THE WERD IS AMBIGUOUS - FOR SPACING WE DONT
    CARE WHETHER WE HAVE of/at on/an etc.
  */
  if ((fixsp_done_mode > 0) &&
    (word->tess_accepted ||
    ((fixsp_done_mode == 2) &&
    (word->reject_map.reject_count () == 0)) ||
    (fixsp_done_mode == 3)) &&
    (strchr (word->best_choice->unichar_string().string (), ' ') == NULL) &&
    ((word->best_choice->permuter () == SYSTEM_DAWG_PERM) ||
    (word->best_choice->permuter () == FREQ_DAWG_PERM) ||
    (word->best_choice->permuter () == USER_DAWG_PERM) ||
    (word->best_choice->permuter () == NUMBER_PERM)))
    return TRUE;
  else
    return FALSE;
}


/*************************************************************************
 * fix_sp_fp_word()
 * Test the current word to see if it can be split by deleting noise blobs. If
 * so, do the buisiness.
 * Return with the iterator pointing to the same place if the word is unchanged,
 * or the last of the replacement words.
 *************************************************************************/
namespace tesseract {
void Tesseract::fix_sp_fp_word(WERD_RES_IT &word_res_it, ROW *row,
                               BLOCK* block) {
  WERD_RES *word_res;
  WERD_RES_LIST sub_word_list;
  WERD_RES_IT sub_word_list_it(&sub_word_list);
  inT16 blob_index;
  inT16 new_length;
  float junk;

  word_res = word_res_it.data ();
  if (!fixsp_check_for_fp_noise_space ||
    word_res->word->flag (W_REP_CHAR) ||
    word_res->combination ||
    word_res->part_of_combo || !word_res->word->flag (W_DONT_CHOP))
    return;

  blob_index = worst_noise_blob (word_res, &junk);
  if (blob_index < 0)
    return;

  #ifndef SECURE_NAMES
  if (debug_fix_space_level > 1) {
    tprintf ("FP fixspace working on \"%s\"\n",
      word_res->best_choice->unichar_string().string());
  }
  #endif
  gblob_sort_list ((PBLOB_LIST *) word_res->word->rej_cblob_list (), FALSE);
  sub_word_list_it.add_after_stay_put (word_res_it.extract ());
  fix_noisy_space_list(sub_word_list, row, block);
  new_length = sub_word_list.length ();
  word_res_it.add_list_before (&sub_word_list);
  for (; (!word_res_it.at_last () && (new_length > 1)); new_length--) {
    word_res_it.forward ();
  }
}

void Tesseract::fix_noisy_space_list(WERD_RES_LIST &best_perm, ROW *row,
                                     BLOCK* block) {
  inT16 best_score;
  WERD_RES_IT best_perm_it(&best_perm);
  WERD_RES_LIST current_perm;
  WERD_RES_IT current_perm_it(&current_perm);
  WERD_RES *old_word_res;
  WERD_RES *new_word_res;
  inT16 current_score;
  BOOL8 improved = FALSE;

                                 //default score
  best_score = fp_eval_word_spacing (best_perm);

  dump_words (best_perm, best_score, 1, improved);

  new_word_res = new WERD_RES;
  old_word_res = best_perm_it.data ();
                                 //Kludge to force deep copy
  old_word_res->combination = TRUE;
  *new_word_res = *old_word_res; //deep copy
                                 //Undo kludge
  old_word_res->combination = FALSE;
                                 //Undo kludge
  new_word_res->combination = FALSE;
  current_perm_it.add_to_end (new_word_res);

  break_noisiest_blob_word(current_perm);

  while ((best_score != PERFECT_WERDS) && !current_perm.empty ()) {
    match_current_words(current_perm, row, block);
    current_score = fp_eval_word_spacing (current_perm);
    dump_words (current_perm, current_score, 2, improved);
    if (current_score > best_score) {
      best_perm.clear ();
      best_perm.deep_copy(&current_perm, &WERD_RES::deep_copy);
      best_score = current_score;
      improved = TRUE;
    }
    if (current_score < PERFECT_WERDS)
      break_noisiest_blob_word(current_perm);
  }
  dump_words (best_perm, best_score, 3, improved);
}
}  // namespace tesseract


/*************************************************************************
 * break_noisiest_blob_word()
 * Find the word with the blob which looks like the worst noise.
 * Break the word into two, deleting the noise blob.
 *************************************************************************/
void break_noisiest_blob_word(WERD_RES_LIST &words) {
  WERD_RES_IT word_it(&words);
  WERD_RES_IT worst_word_it;
  float worst_noise_score = 9999;
  int worst_blob_index = -1;     //noisiest blb of noisiest wd
  int blob_index;                //of wds noisiest blb
  float noise_score;             //of wds noisiest blb
  WERD_RES *word_res;
  C_BLOB_IT blob_it;
  C_BLOB_IT rej_cblob_it;
  C_BLOB_LIST new_blob_list;
  C_BLOB_IT new_blob_it;
  C_BLOB_IT new_rej_cblob_it;
  WERD *new_word;
  inT16 start_of_noise_blob;
  inT16 i;

  for (word_it.mark_cycle_pt (); !word_it.cycled_list (); word_it.forward ()) {
    blob_index = worst_noise_blob (word_it.data (), &noise_score);
    if ((blob_index > -1) && (worst_noise_score > noise_score)) {
      worst_noise_score = noise_score;
      worst_blob_index = blob_index;
      worst_word_it = word_it;
    }
  }
  if (worst_blob_index < 0) {
    words.clear ();              //signal termination
    return;
  }

  /* Now split the worst_word_it */

  word_res = worst_word_it.data ();

  /* Move blobs before noise blob to a new bloblist */

  new_blob_it.set_to_list (&new_blob_list);
  blob_it.set_to_list (word_res->word->cblob_list ());
  for (i = 0; i < worst_blob_index; i++, blob_it.forward ()) {
    new_blob_it.add_after_then_move (blob_it.extract ());
  }
  start_of_noise_blob = blob_it.data ()->bounding_box ().left ();
  delete blob_it.extract ();     //throw out noise blb

  new_word = new WERD (&new_blob_list, word_res->word);
  new_word->set_flag (W_EOL, FALSE);
  word_res->word->set_flag (W_BOL, FALSE);
  word_res->word->set_blanks (1);//After break

  new_rej_cblob_it.set_to_list (new_word->rej_cblob_list ());
  rej_cblob_it.set_to_list (word_res->word->rej_cblob_list ());
  for (;
    (!rej_cblob_it.empty () &&
    (rej_cblob_it.data ()->bounding_box ().left () <
  start_of_noise_blob)); rej_cblob_it.forward ()) {
    new_rej_cblob_it.add_after_then_move (rej_cblob_it.extract ());
  }

  worst_word_it.add_before_then_move (new WERD_RES (new_word));

  word_res->done = FALSE;
  if (word_res->outword != NULL) {
    delete word_res->outword;
    delete word_res->best_choice;
    delete word_res->raw_choice;
    word_res->outword = NULL;
    word_res->best_choice = NULL;
    word_res->raw_choice = NULL;
  }
}


inT16 worst_noise_blob(WERD_RES *word_res, float *worst_noise_score) {
  PBLOB_IT blob_it;
  inT16 blob_count;
  float noise_score[512];
  int i;
  int min_noise_blob;            //1st contender
  int max_noise_blob;            //last contender
  int non_noise_count;
  int worst_noise_blob;          //Worst blob
  float small_limit = bln_x_height * fixsp_small_outlines_size;
  float non_noise_limit = bln_x_height * 0.8;

  blob_it.set_to_list (word_res->outword->blob_list ());
  //normalised
  blob_count = blob_it.length ();
  ASSERT_HOST (blob_count <= 512);
  if (blob_count < 5)
    return -1;                   //too short to split
  /* Get the noise scores for all blobs */

  #ifndef SECURE_NAMES
  if (debug_fix_space_level > 5)
    tprintf ("FP fixspace Noise metrics for \"%s\": ",
      word_res->best_choice->unichar_string().string());
  #endif

  for (i = 0; i < blob_count; i++, blob_it.forward ()) {
    if (word_res->reject_map[i].accepted ())
      noise_score[i] = non_noise_limit;
    else
      noise_score[i] = blob_noise_score (blob_it.data ());

    if (debug_fix_space_level > 5)
      tprintf ("%1.1f ", noise_score[i]);
  }
  if (debug_fix_space_level > 5)
    tprintf ("\n");

  /* Now find the worst one which is far enough away from the end of the word */

  non_noise_count = 0;
  for (i = 0;
  (i < blob_count) && (non_noise_count < fixsp_non_noise_limit); i++) {
    if (noise_score[i] >= non_noise_limit)
      non_noise_count++;
  }
  if (non_noise_count < fixsp_non_noise_limit)
    return -1;
  min_noise_blob = i;

  non_noise_count = 0;
  for (i = blob_count - 1;
  (i >= 0) && (non_noise_count < fixsp_non_noise_limit); i--) {
    if (noise_score[i] >= non_noise_limit)
      non_noise_count++;
  }
  if (non_noise_count < fixsp_non_noise_limit)
    return -1;
  max_noise_blob = i;

  if (min_noise_blob > max_noise_blob)
    return -1;

  *worst_noise_score = small_limit;
  worst_noise_blob = -1;
  for (i = min_noise_blob; i <= max_noise_blob; i++) {
    if (noise_score[i] < *worst_noise_score) {
      worst_noise_blob = i;
      *worst_noise_score = noise_score[i];
    }
  }
  return worst_noise_blob;
}


float blob_noise_score(PBLOB *blob) {
  OUTLINE_IT outline_it;
  TBOX box;                       //BB of outline
  inT16 outline_count = 0;
  inT16 max_dimension;
  inT16 largest_outline_dimension = 0;

  outline_it.set_to_list (blob->out_list ());
  for (outline_it.mark_cycle_pt ();
  !outline_it.cycled_list (); outline_it.forward ()) {
    outline_count++;
    box = outline_it.data ()->bounding_box ();
    if (box.height () > box.width ())
      max_dimension = box.height ();
    else
      max_dimension = box.width ();

    if (largest_outline_dimension < max_dimension)
      largest_outline_dimension = max_dimension;
  }

  if (fixsp_noise_score_fixing) {
    if (outline_count > 5)
                                 //penalise LOTS of blobs
      largest_outline_dimension *= 2;

    box = blob->bounding_box ();

    if ((box.bottom () > bln_baseline_offset * 4) ||
      (box.top () < bln_baseline_offset / 2))
                                 //Lax blob is if high or low
      largest_outline_dimension /= 2;
  }
  return largest_outline_dimension;
}


void fixspace_dbg(WERD_RES *word) {
  TBOX box = word->word->bounding_box ();
  BOOL8 show_map_detail = FALSE;
  inT16 i;

  box.print ();
  #ifndef SECURE_NAMES
  tprintf (" \"%s\" ", word->best_choice->unichar_string().string ());
  tprintf ("Blob count: %d (word); %d/%d (outword)\n",
    word->word->gblob_list ()->length (),
    word->outword->gblob_list ()->length (),
    word->outword->rej_blob_list ()->length ());
  word->reject_map.print (debug_fp);
  tprintf ("\n");
  if (show_map_detail) {
    tprintf ("\"%s\"\n", word->best_choice->unichar_string().string ());
    for (i = 0; word->best_choice->unichar_string()[i] != '\0'; i++) {
      tprintf ("**** \"%c\" ****\n", word->best_choice->unichar_string()[i]);
      word->reject_map[i].full_print (debug_fp);
    }
  }

  tprintf ("Tess Accepted: %s\n", word->tess_accepted ? "TRUE" : "FALSE");
  tprintf ("Done flag: %s\n\n", word->done ? "TRUE" : "FALSE");
  #endif
}


/*************************************************************************
 * fp_eval_word_spacing()
 * Evaluation function for fixed pitch word lists.
 *
 * Basically, count the number of "nice" characters - those which are in tess
 * acceptable words or in dict words and are not rejected.
 * Penalise any potential noise chars
 *************************************************************************/
namespace tesseract {
inT16 Tesseract::fp_eval_word_spacing(WERD_RES_LIST &word_res_list) {
  WERD_RES_IT word_it(&word_res_list);
  WERD_RES *word;
  PBLOB_IT blob_it;
  inT16 word_length;
  inT16 score = 0;
  inT16 i;
  float small_limit = bln_x_height * fixsp_small_outlines_size;

  if (!fixsp_fp_eval)
    return (eval_word_spacing (word_res_list));

  for (word_it.mark_cycle_pt (); !word_it.cycled_list (); word_it.forward ()) {
    word = word_it.data ();
    word_length = word->reject_map.length ();
    if ((word->done ||
      word->tess_accepted) ||
      (word->best_choice->permuter () == SYSTEM_DAWG_PERM) ||
      (word->best_choice->permuter () == FREQ_DAWG_PERM) ||
      (word->best_choice->permuter () == USER_DAWG_PERM) ||
        (safe_dict_word(*(word->best_choice)) > 0)) {
      blob_it.set_to_list (word->outword->blob_list ());
      UNICHAR_ID space = getDict().getUnicharset().unichar_to_id(" ");
      for (i = 0; i < word->best_choice->length(); ++i, blob_it.forward()) {
        if (word->best_choice->unichar_id(i) == space ||
            (blob_noise_score(blob_it.data()) < small_limit)) {
          score -= 1;            //penalise possibly erroneous non-space
        } else if (word->reject_map[i].accepted()) {
          score++;
      }
    }
  }
  }
  if (score < 0)
    score = 0;
  return score;
}
}  // namespace tesseract
