/******************************
 * Trollbot                   *
 ******************************
 * Written by poutine DALnet  *
 ******************************
 * This software is public    *
 * domain. Free for any use   *
 * whatsoever.                *
 ******************************/

#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "main.h"
#include "tconfig.h"

void troll_debug(int level, const char *fmt, ...)
{
  va_list va;
  char buf[2048]; 
  char buf2[2059];

  memset(buf, 0, sizeof(buf));
  memset(buf2, 0, sizeof(buf2));

  va_start(va, fmt);
  vsnprintf(buf, sizeof(buf), fmt, va);
  va_end(va);

  if (config != NULL)
    if (config->debug >= level)
      printf("%s\n",buf);

  return;
}
