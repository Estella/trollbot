/******************************
 * Trollbot                   *
 ******************************
 * Written by poutine DALnet  *
 ******************************
 * This software is public    *
 * domain. Free for any use   *
 * whatsoever.                *
 ******************************/

#include "main.h"

#include "debug.h"

void troll_debug(int level, const char *fmt, ...)
{
  va_list va;
  char buf[2048]; 
  char buf2[2059];

  memset(buf, 0, sizeof(buf));
  memset(buf2, 0, sizeof(buf2));

  va_start(va, fmt);
  /* C99 */
  vsnprintf(buf, sizeof(buf), fmt, va);
  va_end(va);

  if (g_cfg == NULL) 
    printf("%s\n",buf); 
  else if (g_cfg->debug_level >= level && g_cfg->forked == 0)
    printf("%s\n",buf);

  return;
}
