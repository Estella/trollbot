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
#ifndef __LOG_FILTER__
#define __LOG_FILTER__

struct network;
struct log_entry;

struct log_filter
{
	struct network *net;
	char *chan;

	char *flags;

	void (*handler)(struct network *, struct log_filter *, struct log_entry *);

	struct log_filter *prev;
	struct log_filter *next;
};

void log_filters_check(struct log_filter *filters, struct log_entry *entry);
struct log_filter *log_filter_add(struct log_filter *filters, struct log_filter *add);
struct log_filter *log_filter_remove(struct log_filter *filters, struct log_filter *del);
void log_filters_free(struct log_filter *filters);
void log_filter_free(struct log_filter *filter);
struct log_filter *log_filter_new(void);

#endif /* __LOG_FILTER__ */
