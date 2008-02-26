#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdarg.h>

#include "log_entry.h"
#include "debug.h"
#include "config_engine.h"
#include "network.h"
#include "util.h"
#include "main.h"
#include "channel.h"

void log_entry_printf(struct network *net, char *chan, const char *flags, const char *fmt, ...)
{
  va_list va;
  char buf[2048];
	char *ret;
	struct log_entry *entry = NULL;

	if (g_cfg->filters == NULL)
		return;

  memset(buf, 0, sizeof(buf));

  va_start(va, fmt);

  /* C99 */
  vsnprintf(buf, sizeof(buf), fmt, va);
  va_end(va);

	ret = tmalloc0(strlen(buf) + 1);

	strcpy(ret,buf);

	/* We've now got the text in ret, make a log entry, then check filters */
	entry = log_entry_new();

	entry->log_text = ret;
	entry->net      = net;
	entry->chan     = chan;
	entry->flags    = flags;
	
	log_filters_check(g_cfg->filters,entry);

	/* Error, not using dynamic memory */
	entry->flags = NULL;

	log_entry_free(entry);
}

void log_entry_free(struct log_entry *entry)
{
	free(entry->flags);
	free(entry->log_text);

	free(entry);
}

struct log_entry *log_entry_new(void)
{
	struct log_entry *ret = NULL;

	ret = tmalloc(sizeof (struct log_entry));

	ret->net       = NULL;
	ret->chan      = NULL;
	ret->flags     = NULL;
  ret->log_text  = NULL;

	/* Not used, can't think of a reason for a list */
	ret->prev      = NULL;
	ret->next      = NULL;

	return ret;
}
