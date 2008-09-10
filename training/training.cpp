/*
 ** Licensed under the Apache License, Version 2.0 (the "License");
 ** you may not use this file except in compliance with the License.
 ** You may obtain a copy of the License at
 ** http://www.apache.org/licenses/LICENSE-2.0
 ** Unless required by applicable law or agreed to in writing, software
 ** distributed under the License is distributed on an "AS IS" BASIS,
 ** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 ** See the License for the specific language governing permissions and
 ** limitations under the License.
 */

#include "training.h"
#include "debug.h"
#include "memry.h"
#include "scrollview.h"

make_int_var    (LearningDebugLevel, 0, MakeLearningDebugLevel,
        18, 5, SetLearningDebugLevel,
        "Learning Debug Level: ");

make_int_var   (NormMethod, character, MakeNormMethod,
                    15, 10, SetNormMethod,   "Normalization Method   ...")

//char *demodir;                   /*demo home directory */


// Trace printf. Deprecated. Use tprintf instead.
void cprintf(const char* format, ...) {
}

void c_make_current(void* win) {
  ScrollView* window = (ScrollView*) win;
  window->Update();
}

void reverse32(void* ptr) {
  char                    tmp;
  char*                   cptr=(char*)ptr;

  tmp=*cptr;
  *cptr=*(cptr+3);
  *(cptr+3)=tmp;
  tmp=*(cptr+1);
  *(cptr+1)=*(cptr+2);
  *(cptr+2)=tmp;
}

void reverse16(void* ptr) {
  char                    tmp;
  char*                   cptr=(char*)ptr;

  tmp=*cptr;
  *cptr=*(cptr+1);
  *(cptr+1)=tmp;
}

// These are all deprecated! New code should use ScrollView directly.
ScrollView* c_create_window(                     /*create a window*/
                            const char *name,    /*name/title of window*/
                            inT16      xpos,     /*coords of window*/
                            inT16      ypos,     /*coords of window*/
                            inT16      xsize,    /*size of window*/
                            inT16      ysize,    /*size of window*/
                            double     xmin,     /*scrolling limits*/
                            double     xmax,     /*to stop users*/
                            double     ymin,     /*getting lost in*/
                            double     ymax) {   /*empty space*/
 return new ScrollView(name, xpos, ypos, xsize, ysize, xmin + xmax, ymin + ymax,
                       true);
}
void c_line_color_index(void* win, C_COL index) {
  // The colors are the same as the SV ones except that SV has
  // COLOR:NONE --> offset of 1
  ScrollView* window = (ScrollView*) win;
  window->Pen((ScrollView::Color) (index + 1));
}
void c_move(void* win, double x, double y) {
  ScrollView* window = (ScrollView*) win;
  window->SetCursor((int) x, (int) y);
}
void c_draw(void* win, double x, double y) {
  ScrollView* window = (ScrollView*) win;
  window->DrawTo((int) x, (int) y);
}
void c_clear_window(void* win) {
  ScrollView* window = (ScrollView*) win;
  window->Clear();
}
char window_wait(void* win) {
  ScrollView* window = (ScrollView*) win;
  SVEvent* ev;

  // Wait till an input event (all others are thrown away)
  ev = window->AwaitEvent(SVET_ANY);
  char ret = '\0';
  if (ev->type == SVET_INPUT) { ret = ev->parameter[0]; }
  delete ev;
  return ret;
}
