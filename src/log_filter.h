#ifndef __LOG_FILTER__
#define __LOG_FILTER__

struct network;
struct log_entry;

struct log_filter
{
	struct network *net;

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
