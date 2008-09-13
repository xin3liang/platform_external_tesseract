///////////////////////////////////////////////////////////////////////
// File:        tesseractclass.h
// Description: An instance of Tesseract. For thread safety, *every*
//              global variable goes in here, directly, or indirectly.
// Author:      Ray Smith
// Created:     Fri Mar 07 08:17:01 PST 2008
//
// (C) Copyright 2008, Google Inc.
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
///////////////////////////////////////////////////////////////////////

#ifndef TESSERACT_CCMAIN_TESSERACTCLASS_H__
#define TESSERACT_CCMAIN_TESSERACTCLASS_H__

#include "varable.h"
#include "wordrec.h"
#include "ocrclass.h"
#include "control.h"
#include "docqual.h"

class CHAR_SAMPLES_LIST;
class CHAR_SAMPLE_LIST;
class PAGE_RES;
class PAGE_RES_IT;
class BLOCK_LIST;
class IMAGE;
class WERD_RES;
class ROW;
class TBOX;
class SVMenuNode;
struct Pix;
class WERD_CHOICE;
class WERD;
class BLOB_CHOICE_LIST_CLIST;


// Top-level class for all tesseract global instance data.
// This class either holds or points to all data used by an instance
// of Tesseract, including the memory allocator. When this is
// complete, Tesseract will be thread-safe. UNTIL THEN, IT IS NOT!
//
// NOTE to developers: Do not create cyclic dependencies through this class!
// The directory dependency tree must remain a tree! The keep this clean,
// lower-level code (eg in ccutil, the bottom level) must never need to
// know about the content of a higher-level directory.
// The following scheme will grant the easiest access to lower-level
// global members without creating a cyclic dependency:
// ccmain inherits wordrec, includes textord as a member
// wordrec inherits classify
// classify inherits ccstruct, includes dict as a member
// ccstruct inherits c_util, includes image as a member
// c_util inherits cc_util
// textord has a pointer to ccstruct, but doesn't own it.
// dict has a pointer to ccstruct, but doesn't own it.
//
// NOTE: that each level contains members that correspond to global
// data that is defined (and used) at that level, not necessarily where
// the type is defined so for instance:
// BOOL_VAR (textord_show_blobs, FALSE, "Display unsorted blobs");
// goes inside the Textord class, not the cc_util class.

namespace tesseract {

class Tesseract : public Wordrec {
 public:
  Tesseract();
  ~Tesseract();

  void SetBlackAndWhitelist();

  //// control.h /////////////////////////////////////////////////////////
  void recog_all_words(                                //process words
                                  PAGE_RES *page_res,  //page structure
                                                       //progress monitor
                                  volatile ETEXT_DESC *monitor,
                                  TBOX *target_word_box=0L,
                                  inT16 dopasses=0
                                 );
  void classify_word_pass1(                 //recog one word
                           WERD_RES *word,  //word to do
                           ROW *row,
                           BOOL8 cluster_adapt,
                           CHAR_SAMPLES_LIST *char_clusters,
                           CHAR_SAMPLE_LIST *chars_waiting);
  void recog_pseudo_word(                         //recognize blobs
                         BLOCK_LIST *block_list,  //blocks to check
                         TBOX &selection_box);
  void fix_rep_char(WERD_RES *word);
  void fix_quotes(               //make double quotes
                  WERD_CHOICE *choice,  //choice to fix
                  WERD *word,    //word to do //char choices
                  BLOB_CHOICE_LIST_CLIST *blob_choices);
  ACCEPTABLE_WERD_TYPE acceptable_word_string(const char *s,
                                              const char *lengths);
  void match_word_pass2(                 //recog one word
                        WERD_RES *word,  //word to do
                        ROW *row,
                        float x_height);
  void classify_word_pass2(  //word to do
                           WERD_RES *word,
                           ROW *row);
  BOOL8 recog_interactive(            //recognize blobs
                          BLOCK *,    //block
                          ROW *row,   //row of word
                          WERD *word  //word to recognize
                         );
  void fix_hyphens(               //crunch double hyphens
                   WERD_CHOICE *choice,  //choice to fix
                   WERD *word,    //word to do //char choices
                   BLOB_CHOICE_LIST_CLIST *blob_choices);
  void set_word_fonts(
      WERD_RES *word,  // word to adapt to
      BLOB_CHOICE_LIST_CLIST *blob_choices);  // detailed results
  void font_recognition_pass(  //good chars in word
                             PAGE_RES_IT &page_res_it);

  //// output.h //////////////////////////////////////////////////////////

  void output_pass(  //Tess output pass //send to api
                            PAGE_RES_IT &page_res_it,
                            BOOL8 write_to_shm,
                            TBOX *target_word_box);
  FILE *open_outfile(  //open .map & .unlv file
                                const char *extension);
  void write_results(                           //output a word
                     PAGE_RES_IT &page_res_it,  //full info
                     char newline_type,         //type of newline
                     BOOL8 force_eol,           //override tilde crunch?
                     BOOL8 write_to_shm         //send to api
                    );
  void set_unlv_suspects(WERD_RES *word);
  UNICHAR_ID get_rep_char(WERD_RES *word);  // what char is repeated?
  BOOL8 acceptable_number_string(const char *s,
                                 const char *lengths);
  inT16 count_alphanums(const WERD_CHOICE &word);
  inT16 count_alphas(const WERD_CHOICE &word);
  //// tessedit.h ////////////////////////////////////////////////////////
  int init_tesseract(const char *arg0,
                     const char *textbase,
                     const char *language,
                     const char *configfile,
                     int configc,
                     const char *const *configv);

  int init_tesseract_lm(const char *arg0,
                        const char *textbase,
                        const char *language,
                        const char *configfile,
                        int configc,
                        const char *const *configv);

  void recognize_page(STRING& image_name);
  void end_tesseract();

  void set_tess_tweak_vars();

  void init_tesseract_lang_data(const char *arg0,
                                const char *textbase,
                                const char *language,
                                const char *configfile,
                                int configc,
                                const char *const *configv);
  //// pgedit.h //////////////////////////////////////////////////////////
  SVMenuNode *build_menu_new();
  void pgeditor_main(BLOCK_LIST *blocks);
#ifndef GRAPHICS_DISABLED
  void process_image_event( // action in image win
                           const SVEvent &event);
#endif
  void pgeditor_read_file(                   // of serialised file
                          STRING &filename,
                          BLOCK_LIST *blocks  // block list to add to
                         );
  void do_new_source(           // serialise
                    );
  BOOL8 process_cmd_win_event(                 // UI command semantics
                              inT32 cmd_event,  // which menu item?
                              char *new_value   // any prompt data
                             );
  //// reject.h //////////////////////////////////////////////////////////
  BOOL8 ambig_word(const char *start_word,
                   char *temp_word,     //alterable copy
                   inT16 test_char_pos  //idx to char to alter
                  );
  const char *char_ambiguities(char c);
  void make_reject_map(            //make rej map for wd //detailed results
                       WERD_RES *word,
                       BLOB_CHOICE_LIST_CLIST *blob_choices,
                       ROW *row,
                       inT16 pass  //1st or 2nd?
                      );
#ifndef EMBEDDED
  void test_ambigs(const char *word);
#endif
  BOOL8 one_ell_conflict(WERD_RES *word_res, BOOL8 update_map);
  inT16 first_alphanum_index(const char *word,
                             const char *word_lengths);
  inT16 first_alphanum_offset(const char *word,
                              const char *word_lengths);
  inT16 alpha_count(const char *word,
                    const char *word_lengths);
  BOOL8 word_contains_non_1_digit(const char *word,
                                  const char *word_lengths);
  void dont_allow_1Il(WERD_RES *word);
  inT16 count_alphanums(  //how many alphanums
                        WERD_RES *word);
  BOOL8 repeated_ch_string(const char *rep_ch_str,
                           const char *lengths);
  void flip_0O(WERD_RES *word);
  BOOL8 non_0_digit(UNICHAR_ID unichar_id);
  BOOL8 non_O_upper(UNICHAR_ID unichar_id);
  BOOL8 repeated_nonalphanum_wd(WERD_RES *word, ROW *row);
  void nn_match_word(  //Match a word
                     WERD_RES *word,
                     ROW *row);
  void nn_recover_rejects(WERD_RES *word, ROW *row);
  BOOL8 test_ambig_word(  //test for ambiguity
                        WERD_RES *word);
  void set_done(  //set done flag
                WERD_RES *word,
                inT16 pass);
  inT16 safe_dict_word(const char *s);
  void flip_hyphens(WERD_RES *word);
  //// adaptions.h ///////////////////////////////////////////////////////
  void adapt_to_good_ems(WERD_RES *word,
                         CHAR_SAMPLES_LIST *char_clusters,
                         CHAR_SAMPLE_LIST *chars_waiting);
  void adapt_to_good_samples(WERD_RES *word,
                             CHAR_SAMPLES_LIST *char_clusters,
                             CHAR_SAMPLE_LIST *chars_waiting);

  BOOL_VAR_H(tessedit_resegment_from_boxes, false,
             "Take segmentation and labeling from box file");
  BOOL_VAR_H(tessedit_train_from_boxes, false,
             "Generate training data from boxed chars");
  STRING_VAR_H(tessedit_char_blacklist, "",
               "Blacklist of chars not to recognize");
  STRING_VAR_H(tessedit_char_whitelist, "",
               "Whitelist of chars to recognize");
  BOOL8 word_adaptable(  //should we adapt?
                       WERD_RES *word,
                       uinT16 mode);
  void reject_suspect_ems(WERD_RES *word);
  void collect_ems_for_adaption(WERD_RES *word,
                                CHAR_SAMPLES_LIST *char_clusters,
                                CHAR_SAMPLE_LIST *chars_waiting);
  void collect_characters_for_adaption(WERD_RES *word,
                                       CHAR_SAMPLES_LIST *char_clusters,
                                       CHAR_SAMPLE_LIST *chars_waiting);
  void check_wait_list(CHAR_SAMPLE_LIST *chars_waiting,
                       CHAR_SAMPLE *sample,
                       CHAR_SAMPLES *best_cluster);
  void cluster_sample(CHAR_SAMPLE *sample,
                      CHAR_SAMPLES_LIST *char_clusters,
                      CHAR_SAMPLE_LIST *chars_waiting);
  void complete_clustering(CHAR_SAMPLES_LIST *char_clusters,
                           CHAR_SAMPLE_LIST *chars_waiting);

  //// tfacepp.cpp ///////////////////////////////////////////////////////
  WERD_CHOICE *recog_word_recursive(                    //recog one owrd
                                    WERD *word,         //word to do
                                    DENORM *denorm,     //de-normaliser
                                                        //matcher function
                                    POLY_MATCHER matcher,
                                                        //tester function
                                    POLY_TESTER tester,
                                                        //trainer function
                                    POLY_TESTER trainer,
                                    BOOL8 testing,      //true if answer driven
                                                        //raw result
                                    WERD_CHOICE *&raw_choice,
                                                        //list of blob lists
                                    BLOB_CHOICE_LIST_CLIST *blob_choices,
                                    WERD *&outword      //bln word output
                                   );
  WERD_CHOICE *recog_word(                           //recog one owrd
                          WERD *word,                //word to do
                          DENORM *denorm,            //de-normaliser
                          POLY_MATCHER matcher,      //matcher function
                          POLY_TESTER tester,        //tester function
                          POLY_TESTER trainer,       //trainer function
                          BOOL8 testing,             //true if answer driven
                          WERD_CHOICE *&raw_choice,  //raw result
                                                     //list of blob lists
                          BLOB_CHOICE_LIST_CLIST *blob_choices,
                          WERD *&outword             //bln word output
                         );
  WERD_CHOICE *split_and_recog_word(                    //recog one owrd
                                    WERD *word,         //word to do
                                    DENORM *denorm,     //de-normaliser
                                                        //matcher function
                                    POLY_MATCHER matcher,
                                                        //tester function
                                    POLY_TESTER tester,
                                                        //trainer function
                                    POLY_TESTER trainer,
                                    BOOL8 testing,      //true if answer driven
                                                        //raw result
                                    WERD_CHOICE *&raw_choice,
                                                        //list of blob lists
                                    BLOB_CHOICE_LIST_CLIST *blob_choices,
                                    WERD *&outword      //bln word output
                                   );
  //// fixspace.cpp ///////////////////////////////////////////////////////
  BOOL8 digit_or_numeric_punct(WERD_RES *word, int char_position);
  inT16 eval_word_spacing(WERD_RES_LIST &word_res_list);
  void match_current_words(WERD_RES_LIST &words, ROW *row);
  inT16 fp_eval_word_spacing(WERD_RES_LIST &word_res_list);
  void fix_noisy_space_list(WERD_RES_LIST &best_perm, ROW *row);
  void fix_fuzzy_space_list(  //space explorer
                            WERD_RES_LIST &best_perm,
                            ROW *row);
  void fix_sp_fp_word(WERD_RES_IT &word_res_it, ROW *row);
  void fix_fuzzy_spaces(                               //find fuzzy words
                        volatile ETEXT_DESC *monitor,  //progress monitor
                        inT32 word_count,              //count of words in doc
                        PAGE_RES *page_res);
  //// docqual.cpp ////////////////////////////////////////////////////////
  GARBAGE_LEVEL garbage_word(WERD_RES *word, BOOL8 ok_dict_word);
  BOOL8 potential_word_crunch(WERD_RES *word,
                              GARBAGE_LEVEL garbage_level,
                              BOOL8 ok_dict_word);
  void tilde_crunch(PAGE_RES_IT &page_res_it);
  void unrej_good_quality_words(  //unreject potential
                                PAGE_RES_IT &page_res_it);
  void doc_and_block_rejection(  //reject big chunks
                               PAGE_RES_IT &page_res_it,
                               BOOL8 good_quality_doc);
  void quality_based_rejection(PAGE_RES_IT &page_res_it,
                               BOOL8 good_quality_doc);
  void convert_bad_unlv_chs(WERD_RES *word_res);
  void merge_tess_fails(WERD_RES *word_res);
  void tilde_delete(PAGE_RES_IT &page_res_it);
  void insert_rej_cblobs(WERD_RES *word);
  //// pagewalk.cpp ///////////////////////////////////////////////////////
  void
  process_selected_words (
      BLOCK_LIST * block_list, //blocks to check
      //function to call
      TBOX & selection_box,
      BOOL8 (tesseract::Tesseract::*word_processor) (
          BLOCK *,
          ROW *,
          WERD *));
  //// tessbox.cpp ///////////////////////////////////////////////////////
  void tess_add_doc_word(                          //test acceptability
                         WERD_CHOICE *word_choice  //after context
                        );
  void tess_adapter(                         //adapt to word
                    WERD *word,              //bln word
                    DENORM *denorm,          //de-normalise
                    const WERD_CHOICE& choice,      //string for word
                    const WERD_CHOICE& raw_choice,  //before context
                    const char *rejmap       //reject map
                   );
  WERD_CHOICE *test_segment_pass2(                        //recog one word
                                  WERD *word,             //bln word to do
                                  DENORM *denorm,         //de-normaliser
                                  POLY_MATCHER matcher,   //matcher function
                                  POLY_TESTER tester,     //tester function
                                                          //raw result
                                  WERD_CHOICE *&raw_choice,
                                                          //list of blob lists
                                  BLOB_CHOICE_LIST_CLIST *blob_choices,
                                  WERD *&outword          //bln word output
                                 );
  WERD_CHOICE *tess_segment_pass1(                        //recog one word
                                  WERD *word,             //bln word to do
                                  DENORM *denorm,         //de-normaliser
                                  POLY_MATCHER matcher,   //matcher function
                                                          //raw result
                                  WERD_CHOICE *&raw_choice,
                                                          //list of blob lists
                                  BLOB_CHOICE_LIST_CLIST *blob_choices,
                                  WERD *&outword             //bln word output
                                 );
  WERD_CHOICE *tess_segment_pass2(                        //recog one word
                                  WERD *word,             //bln word to do
                                  DENORM *denorm,         //de-normaliser
                                  POLY_MATCHER matcher,   //matcher function
                                                          //raw result
                                  WERD_CHOICE *&raw_choice,
                                                          //list of blob lists
                                  BLOB_CHOICE_LIST_CLIST *blob_choices,
                                  WERD *&outword          //bln word output
                                 );
  WERD_CHOICE *correct_segment_pass2(                       //recog one word
                                     WERD *word,            //bln word to do
                                     DENORM *denorm,        //de-normaliser
                                     POLY_MATCHER matcher,  //matcher function
                                     POLY_TESTER tester,    //tester function
                                                            //raw result
                                     WERD_CHOICE *&raw_choice,
                                                            //list of blob lists
                                     BLOB_CHOICE_LIST_CLIST *blob_choices,
                                     WERD *&outword         //bln word output
                                    );
  void tess_default_matcher(                            //call tess
                            PBLOB *pblob,               //previous blob
                            PBLOB *blob,                //blob to match
                            PBLOB *nblob,               //next blob
                            WERD *word,                 //word it came from
                            DENORM *denorm,             //de-normaliser
                            BLOB_CHOICE_LIST *ratings,  //list of results
                            const char* script
                            );
  void tess_bn_matcher(                           //call tess
                       PBLOB *pblob,              //previous blob
                       PBLOB *blob,               //blob to match
                       PBLOB *nblob,              //next blob
                       WERD *word,                //word it came from
                       DENORM *denorm,            //de-normaliser
                       BLOB_CHOICE_LIST *ratings  //list of results
                      );
  void tess_cn_matcher(                           //call tess
                       PBLOB *pblob,              //previous blob
                       PBLOB *blob,               //blob to match
                       PBLOB *nblob,              //next blob
                       WERD *word,                //word it came from
                       DENORM *denorm,            //de-normaliser
                       BLOB_CHOICE_LIST *ratings  //list of results
                      );
  BOOL8 tess_adaptable_word(                           //test adaptability
                            WERD *word,                //word to test
                            WERD_CHOICE *word_choice,  //after context
                            WERD_CHOICE *raw_choice    //before context
                           );
  BOOL8 tess_acceptable_word(                           //test acceptability
                             WERD_CHOICE *word_choice,  //after context
                             WERD_CHOICE *raw_choice    //before context
                            );
  //// applybox.cpp //////////////////////////////////////////////////////
  void apply_box_testing(BLOCK_LIST *block_list);
  void apply_boxes(const STRING& fname,
                   BLOCK_LIST *block_list    //real blocks
                  );
  //// blobcmp.cpp ///////////////////////////////////////////////////////
  float compare_tess_blobs(TBLOB *blob1,
                           TEXTROW *row1,
                           TBLOB *blob2,
                           TEXTROW *row2);
  //// paircmp.cpp ///////////////////////////////////////////////////////
  float compare_bln_blobs(               //match 2 blobs
                          PBLOB *blob1,  //first blob
                          DENORM *denorm1,
                          PBLOB *blob2,  //other blob
                          DENORM *denorm2);
  float compare_blobs(               //match 2 blobs
                      PBLOB *blob1,  //first blob
                      ROW *row1,     //row it came from
                      PBLOB *blob2,  //other blob
                      ROW *row2);
  BOOL8 compare_blob_pairs(             //blob processor
                           BLOCK *,
                           ROW *row,    //row it came from
                           WERD *,
                           PBLOB *blob  //blob to compare
                          );
  //// fixxht.cpp ///////////////////////////////////////////////////////
  void check_block_occ(WERD_RES *word_res);
};

}  // namespace tesseract


#endif  // TESSERACT_CCMAIN_TESSERACTCLASS_H__
