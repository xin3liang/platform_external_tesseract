/**********************************************************************
 * File:        pagesegmain.cpp
 * Description: Top-level page segmenter for Tesseract.
 * Author:      Ray Smith
 * Created:     Thu Sep 25 17:12:01 PDT 2008
 *
 * (C) Copyright 2008, Google Inc.
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

// Include automatically generated configuration file if running autoconf.
#ifdef HAVE_CONFIG_H
#include "config_auto.h"
#endif

#ifdef HAVE_LIBLEPT
// Include leptonica library only if autoconf (or makefile etc) tell us to.
#include "allheaders.h"
#endif

#include "tesseractclass.h"
#include "img.h"
#include "blobbox.h"
#include "blread.h"
#include "wordseg.h"
#include "makerow.h"
#include "baseapi.h"
#include "tordmain.h"

namespace tesseract {

// Minimum believable resolution.
const int kMinCredibleResolution = 70;
// Default resolution used if input in not believable.
const int kDefaultResolution = 300;

// Segment the page according to the current value of tessedit_pageseg_mode.
// If the input pix is not NULL, it is used as the source image, and copied
// to image, otherwise it just uses image as the input.
// On return the blocks list owns all the constructed page layout.
void Tesseract::SegmentPage(const STRING* input_file,
                            Pix* pix, IMAGE* image, BLOCK_LIST* blocks) {
  int width = image->get_xsize();
  int height = image->get_ysize();
  int resolution = image->get_res();
#ifdef HAVE_LIBLEPT
  if (pix != NULL) {
    width = pixGetWidth(pix);
    height = pixGetHeight(pix);
    resolution = pixGetXRes(pix);
    image->FromPix(pix);
  }
#endif
  // Zero resolution messes up the algorithms, so make sure it is credible.
  if (resolution < kMinCredibleResolution)
    resolution = kDefaultResolution;
  // If a UNLV zone file can be found, use that instead of segmentation.
  if (input_file != NULL && input_file->length() > 0) {
    STRING name = *input_file;
    const char* lastdot = strrchr(name.string(), '.');
    if (lastdot != NULL)
      name[lastdot - name.string()] = '\0';
    read_pd_file(name, width, height, blocks);
  }
  if (blocks->empty()) {
    // No UNLV file present. Work according to the PageSegMode.
    BLOCK_IT block_it(blocks);
    BLOCK* block = new BLOCK("", TRUE, 0, 0, 0, 0, width, height);
    block_it.add_to_end(block);
    // TODO(rays) Add call to AutoPageSeg here.
  }
  TO_BLOCK_LIST land_blocks, port_blocks;
  TBOX page_box;
  find_components(blocks, &land_blocks, &port_blocks, &page_box);

  TO_BLOCK_IT to_block_it(&port_blocks);
  TO_BLOCK* to_block = to_block_it.data();
  if (tessedit_pageseg_mode <= PSM_SINGLE_BLOCK ||
      to_block->line_size < 2) {
    // For now, AUTO, SINGLE_COLUMN and SINGLE_BLOCK all map to the
    // old textord.
    textord_page(page_box.topright(), blocks, &land_blocks, &port_blocks,
                 this);
  } else {
    // SINGLE_LINE, SINGLE_WORD and SINGLE_CHAR all need a single row.
    float gradient = make_single_row(page_box.topright(),
                                     to_block, &port_blocks);
    if (tessedit_pageseg_mode == PSM_SINGLE_LINE) {
      // SINGLE_LINE uses the old word maker on the single line.
      make_words(page_box.topright(), gradient, blocks,
                 &land_blocks, &port_blocks, this);
    } else {
      // SINGLE_WORD and SINGLE_CHAR cram all the blobs into a
      // single word, and in SINGLE_CHAR mode, all the outlines
      // go in a single blob.
      make_single_word(tessedit_pageseg_mode == PSM_SINGLE_CHAR,
                       to_block->get_rows(), to_block->block->row_list());
    }
  }
}

}  // namespace tesseract.

