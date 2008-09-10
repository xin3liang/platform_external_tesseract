///////////////////////////////////////////////////////////////////////
// File:        ccutil.h
// Description: ccutil class.
// Author:      Samuel Charron
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

#ifndef TESSERACT_CCUTIL_CCUTIL_H__
#define TESSERACT_CCUTIL_CCUTIL_H__

#include "errcode.h"
#include "strngs.h"
#include "varable.h"
#include "unicharset.h"

#ifdef WIN32
#include <windows.h>
#else
#include <pthread.h>
#include <semaphore.h>
#endif

namespace tesseract {

class CCUtilMutex {
 public:
  CCUtilMutex();

  void Lock();

  void Unlock();
 private:
#ifdef WIN32
  HANDLE mutex_;
#else
  pthread_mutex_t mutex_;
#endif
};


class CCUtil {
 public:
  CCUtil();
  ~CCUtil();


  // Memory Allocator there
  //
  //

  //// mainblk.* //////////////////////////////////////////////////////
 public:
  void main_setup(                         /*main demo program */
                  const char *argv0,       //program name
                  const char *basename,    //name of image
                  int argc,                /*argument count */
                  const char *const *argv  /*arguments */
                 );
 public:
  STRING datadir;    //dir for data files
                                 //name of image
  STRING imagebasename;

  BOOL_VAR_H (m_print_variables, FALSE,
                   "Print initial values of all variables");
  STRING_VAR_H (m_data_sub_dir, "tessdata/", "Directory for data files");
  STRING language_data_path_prefix;
  UNICHARSET unicharset;
  STRING imagefile;    /*image file name */
  STRING directory;    /* main directory */
};

extern CCUtilMutex tprintfMutex;
}  // namespace tesseract

#endif  // TESSERACT_CCUTIL_CCUTIL_H__
