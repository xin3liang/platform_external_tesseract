///////////////////////////////////////////////////////////////////////
// File:        tesseractclass.cpp
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

#include "tesseractclass.h"
#include "globals.h"

namespace tesseract {

Tesseract::Tesseract()
  : BOOL_MEMBER(tessedit_resegment_from_boxes, false,
                "Take segmentation and labeling from box file"),
    BOOL_MEMBER(tessedit_train_from_boxes, false,
                "Generate training data from boxed chars"),
    INT_MEMBER(tessedit_pageseg_mode, 0,
               "Page seg mode: 0=auto, 1=col, 2=block, 3=line, 4=word, 6=char"),
    STRING_MEMBER(tessedit_char_blacklist, "",
                  "Blacklist of chars not to recognize"),
    STRING_MEMBER(tessedit_char_whitelist, "",
                  "Whitelist of chars to recognize") {
}

Tesseract::~Tesseract() {
}

void Tesseract::SetBlackAndWhitelist() {
  // Set the white and blacklists (if any)
  unicharset.set_black_and_whitelist(tessedit_char_blacklist.string(),
                                     tessedit_char_whitelist.string());
}

}  // namespace tesseract
