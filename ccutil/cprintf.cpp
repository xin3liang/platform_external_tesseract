#include          "platform.h"
#ifdef __UNIX__
#include <stdarg.h>
#endif
#include          "tprintf.h"

void
cprintf (                        //Trace printf
const char *format, ...          //special message
) {
  va_list args;                  //variable args
  char msg[1000];

  va_start(args, format);  //variable list
  vsprintf(msg, format, args);  //Format into msg
  va_end(args);

  tprintf ("%s", msg);
}


