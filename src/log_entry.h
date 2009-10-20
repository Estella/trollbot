/*********************************************
 * TrollBot v1.0                             *
 *********************************************
 * TrollBot is an eggdrop-clone designed to  *
 * work with multiple networks and protocols *
 * in order to present a unified scriptable  *
 * event-based platform,                     *
 *********************************************
 * This software is PUBLIC DOMAIN. Feel free *
 * to use it for whatever use whatsoever.    *
 *********************************************
 * Originally written by poutine/DALnet      *
 *                       kicken/DALnet       *
 *                       comcor/DALnet       *
 *********************************************/
#ifndef __LOG_ENTRY__
#define __LOG_ENTRY__

struct network;
struct log_entry;

struct log_entry
{
	struct network *net;
	char *chan;

	char *log_text;

	char *flags;

	struct log_entry *prev;
	struct log_entry *next;
};

void log_entry_printf(struct network *net, char *chan, const char *flags, const char *fmt, ...);
void log_entry_free(struct log_entry *entry);
struct log_entry *log_entry_new(void);

#endif /* __LOG_ENTRY__ */
