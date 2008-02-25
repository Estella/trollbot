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
#include "log_filter.h"
#include "network.h"
#include "log_entry.h"
#include "util.h"


void console_log_filter_handler(struct network *net, struct log_filter *filter, struct log_entry *entry)
{
	printf("%s\n",entry->log_text);
}

/* This method is to be replaced with
 * eggdrop's flag based logs.
 *
 * I'd like to create a log transport
 * "object" of sorts to create log filters
 * that be used for IRC, email, etc.
 *
 * plus wtf was I thinking with these static buffers.
 * I'm surprised the DCC MOTD addition isn't shitting
 * itself.
 *
 * Today was a sunny day, I was criticized for using
 * two cups for my coffee at the convenience store
 * today. The lady said she will start charging me
 * for two coffees despite the actual content of the
 * cups. I explained to her that the cups were cheap
 * and that the coffee retained its warmth a great
 * amount of time more than with a single piece of
 * shit flimsy cup.
 */
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
