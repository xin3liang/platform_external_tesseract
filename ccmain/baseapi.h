///////////////////////////////////////////////////////////////////////
// File:        baseapi.h
// Description: Simple API for calling tesseract.
// Author:      Ray Smith
// Created:     Fri Oct 06 15:35:01 PDT 2006
//
// (C) Copyright 2006, Google Inc.
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

#ifndef TESSERACT_CCMAIN_BASEAPI_H__
#define TESSERACT_CCMAIN_BASEAPI_H__

#include "thresholder.h"

class PAGE_RES;
class BLOCK_LIST;
class IMAGE;
class STRING;
struct Pix;
struct ETEXT_STRUCT;
struct OSResults;

namespace tesseract {

class Tesseract;
class Dict;

typedef int (Dict::*DictFunc)(void* dawg, void* node, int char_index,
                              char prevchar, const char *word, int word_end);

// Base class for all tesseract APIs.
// Specific classes can add ability to work on different inputs or produce
// different outputs.
// This class is mostly an interface layer on top of the Tesseract instance
// class to hide the data types so that users of this class don't have to
// include any other Tesseract headers.

class TessBaseAPI {
 public:
  TessBaseAPI();
  virtual ~TessBaseAPI();

  // Set the name of the input file. Needed only for training and
  // reading a UNLV zone file.
  void SetInputName(const char* name);

  // Set the name of the bonus output files. Needed only for debugging.
  void SetOutputName(const char* name);

  // Set the value of an internal "variable" (of either old or new types).
  // Supply the name of the variable and the value as a string, just as
  // you would in a config file.
  // Returns false if the name lookup failed.
  // Eg SetVariable("tessedit_char_blacklist", "xyz"); to ignore x, y and z.
  // Or SetVariable("bln_numericmode", "1"); to set numeric-only mode.
  // SetVariable may be used before Init, but settings will revert to
  // defaults on End().
  bool SetVariable(const char* variable, const char* value);

  // Eventually instances will be thread-safe and totally independent,
  // but for now, they all point to the same underlying engine,
  // and are NOT RE-ENTRANT OR THREAD-SAFE. For now:
  // it is safe to Init multiple TessBaseAPIs in the same language, use them
  // sequentially, and End or delete them all, but once one is Ended, you can't
  // do anything other than End the others. After End, it is safe to Init
  // again on the same one.
  //
  // Start tesseract. Returns zero on success. If initialization fails, a
  // non-zero error code will be returned in future, but it currently exits.
  // NOTE that the only members that may be called before Init are those
  // listed above here in the class definition.
  //
  // The datapath must be the name of the data directory (no ending /) or
  // some other file in which the data directory resides (for instance argv[0].)
  // The language is (usually) an ISO 639-3 string or NULL will default to eng.
  // It is entirely safe (and eventually will be efficient too) to call
  // Init multiple times on the same instance to change language, or just
  // to reset the classifier.
  // WARNING: On changing languages, all Variables are reset back to their
  // default values. If you have a rare need to set a Variable that controls
  // initialization for a second call to Init you should explicitly
  // call End() and then use SetVariable before Init. This is only a very
  // rare use case, since there are very few uses that require any variables
  // to be set before Init.
  int Init(const char* datapath, const char* language);

  // Init only the lang model component of Tesseract. The only functions
  // that work after this init are SetVariable and IsValidWord.
  // WARNING: temporary! This function will be removed from here and placed
  // in a separate API at some future time.
  int InitLangMod(const char* datapath, const char* language);

  // Read a "config" file containing a set of variable, value pairs.
  // Searches the standard places: tessdata/configs, tessdata/tessconfigs
  // and also accepts a relative or absolute path name.
  bool ReadConfigFile(const char* filename);

  // Recognize a rectangle from an image and return the result as a string.
  // May be called many times for a single Init.
  // Currently has no error checking.
  // Greyscale of 8 and color of 24 or 32 bits per pixel may be given.
  // Palette color images will not work properly and must be converted to
  // 24 bit.
  // Binary images of 1 bit per pixel may also be given but they must be
  // byte packed with the MSB of the first byte being the first pixel, and a
  // 1 represents WHITE. For binary images set bytes_per_pixel=0.
  // The recognized text is returned as a char* which is coded
  // as UTF8 and must be freed with the delete [] operator.
  //
  // Note that TesseractRect is the simplified convenience interface.
  // For advanced uses, use SetImage, (optionally) SetRectangle, Recognize,
  // and one or more of the Get*Text functions below.
  char* TesseractRect(const unsigned char* imagedata,
                      int bytes_per_pixel, int bytes_per_line,
                      int left, int top, int width, int height);

  // Call between pages or documents etc to free up memory and forget
  // adaptive data.
  void ClearAdaptiveClassifier();

  // ------------------------Advanced API--------------------------------
  // The following methods break TesseractRect into pieces, so you can
  // get hold of the thresholded image, get the text in different formats,
  // get bounding boxes, confidences etc.

  // Provide an image for Tesseract to recognize. Format is as
  // TesseractRect above. Does not copy the image buffer, or take
  // ownership. The source image may be destroyed after Recognize is called,
  // either explicitly or implicitly via one of the Get*Text functions.
  // SetImage clears all recognition results, and sets the rectangle to the
  // full image, so it may be followed immediately by a GetUTF8Text, and it
  // will automatically perform recognition.
  void SetImage(const unsigned char* imagedata, int width, int height,
                int bytes_per_pixel, int bytes_per_line);

#ifdef HAVE_LIBLEPT
  // Provide an image for Tesseract to recognize. As with SetImage above,
  // Tesseract doesn't take a copy or ownership or pixDestroy the image, so
  // it must persist until after Recognize.
  // Pix vs raw, which to use?
  // Use Pix where possible. A future version of Tesseract may choose to use Pix
  // as its internal representation and discard IMAGE altogether.
  // Because of that, an implementation that sources and targets Pix may end up
  // with less copies than an implementation that does not.
  void SetImage(const Pix* pix);
#endif

  // Restrict recognition to a sub-rectangle of the image. Call after SetImage.
  // Each SetRectangle clears the recogntion results so multiple rectangles
  // can be recognized with the same image.
  void SetRectangle(int left, int top, int width, int height);

  // In extreme cases only, usually with a subclass of Thresholder, it
  // is possible to provide a different Thresholder. The Thresholder may
  // be preloaded with an image, settings etc, or they may be set after.
  // Note that Tesseract takes ownership of the Thresholder and will
  // delete it when it it is replaced or the API is destructed.
  void SetThresholder(ImageThresholder* thresholder) {
    if (thresholder_ != NULL)
      delete thresholder_;
    thresholder_ = thresholder;
    ClearResults();
  }

#ifdef HAVE_LIBLEPT
  // Get a copy of the internal thresholded image from Tesseract.
  // Caller takes ownership of the Pix and must pixDestroy it.
  // May be called any time after SetImage, or after TesseractRect.
  Pix* GetThresholdedImage();
#endif

  // Dump the internal binary image to a PGM file.
  // Deprecated. Use GetThresholdedImage and write the image using pixWrite
  // instead if possible.
  void DumpPGM(const char* filename);

  // Recognize the image from SetAndThresholdImage, generating Tesseract
  // internal structures. Returns 0 on success.
  // Optional. The Get*Text functions below will call Recognize if needed.
  // After Recognize, the output is kept internally until the next SetImage.
  int Recognize(ETEXT_STRUCT* monitor);

  // Methods to retrieve information after SetAndThresholdImage(),
  // Recognize() or TesseractRect(). (Recognize is called implicitly if needed.)

  // The recognized text is returned as a char* which is coded
  // as UTF8 and must be freed with the delete [] operator.
  char* GetUTF8Text();
  // The recognized text is returned as a char* which is coded in the same
  // format as a box file used in training. Returned string must be freed with
  // the delete [] operator.
  // Constructs coordinates in the original image - not just the rectangle.
  char* GetBoxText();
  // The recognized text is returned as a char* which is coded
  // as UNLV format Latin-1 with specific reject and suspect codes
  // and must be freed with the delete [] operator.
  char* GetUNLVText();
  // Returns the (average) confidence value between 0 and 100.
  int MeanTextConf();
  // Returns all word confidences (between 0 and 100) in an array, terminated
  // by -1.  The calling function must delete [] after use.
  // The number of confidences should correspond to the number of space-
  // delimited words in GetUTF8Text.
  int* AllWordConfidences();

  // Free up recognition results and any stored image data, without actually
  // freeing any recognition data that would be time-consuming to reload.
  // Afterwards, you must call SetImage or TesseractRect before doing
  // any Recognize or Get* operation.
  void Clear();

  // Close down tesseract and free up all memory. End() is equivalent to
  // destructing and reconstructing your TessBaseAPI.
  // Once End() has been used, none of the other API functions may be used
  // other than Init and anything declared above it in the class definition.
  void End();

  // Check whether a word is valid according to Tesseract's language model
  // returns 0 if the word is invalid, non-zero if valid.
  // WARNING: temporary! This function will be removed from here and placed
  // in a separate API at some future time.
  int IsValidWord(const char *word);

  bool GetTextDirection(int* out_offset, float* out_slope);

  // Set the letter_is_okay function to point somewhere else.
  void SetDictFunc(DictFunc f);

  // Return the Orientation And Script
  void DetectOS(OSResults*);

 protected:
  // Common code for setting the image. Returns true if Init has been called.
  bool InternalSetImage();

  // Run the thresholder to make the thresholded image.
  virtual void Threshold();

  // Find lines from the image making the BLOCK_LIST.
  // Returns 0 on success.
  int FindLines();

  // Delete the pageres and block list ready for a new page.
  void ClearResults();

  // Return the length of the output text string, as UTF8, assuming
  // one newline per line and one per block, with a terminator,
  // and assuming a single character reject marker for each rejected character.
  // Also return the number of recognized blobs in blob_count.
  int TextLength(int* blob_count);

  // __________________________   ocropus add-ons   ___________________________

  // Find lines from the image making the BLOCK_LIST.
  BLOCK_LIST* FindLinesCreateBlockList();

  // Delete a block list.
  // This is to keep BLOCK_LIST pointer opaque
  // and let go of including the other headers.
  static void DeleteBlockList(BLOCK_LIST* block_list);

  // Adapt to recognize the current image as the given character.
  // The image must be preloaded and be just an image of a single character.
  void AdaptToCharacter(const char *unichar_repr,
                        int length,
                        float baseline,
                        float xheight,
                        float descender,
                        float ascender);

  // Recognize text doing one pass only, using settings for a given pass.
  /*static*/ PAGE_RES* RecognitionPass1(BLOCK_LIST* block_list);
  /*static*/ PAGE_RES* RecognitionPass2(BLOCK_LIST* block_list,
                                    PAGE_RES* pass1_result);

  // Extract the OCR results, costs (penalty points for uncertainty),
  // and the bounding boxes of the characters.
  static int TesseractExtractResult(char** text,
                                    int** lengths,
                                    float** costs,
                                    int** x0,
                                    int** y0,
                                    int** x1,
                                    int** y1,
                                    PAGE_RES* page_res);

 protected:
  Tesseract*  tesseract_;          // The underlying data object.
  ImageThresholder* thresholder_;  // The image thresholding module.
  bool threshold_done_;            // The image has been passed to page_image.
  BLOCK_LIST* block_list_;         // The page layout.
  PAGE_RES*   page_res_;           // The page-level data.
  STRING*     input_file_;         // Name used by training code.
  STRING*     output_file_;        // Name used by debug code.
  STRING*     datapath_;           // Current location of tessdata.
  STRING*     language_;           // Last initialized language.
  // Parameters saved from the Thresholder. Needed to rebuild coordinates.
  int rect_left_;
  int rect_top_;
  int rect_width_;
  int rect_height_;
  int image_width_;
  int image_height_;
};

}  // namespace tesseract.

#endif  // TESSERACT_CCMAIN_BASEAPI_H__
