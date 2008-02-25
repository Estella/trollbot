#ifndef __LOG_ENTRY__
#define __LOG_ENTRY__

struct network;
struct log_entry;

struct log_entry
{
	struct network *net;

	char *log_text;

	char *flags;

	struct log_entry *prev;
	struct log_entry *next;
};

void log_entry_sprintf(struct network *net, const char *flags, const char *fmt, ...);
void log_entry_free(struct log_entry *entry);
struct log_entry *log_entry_new(void);

#endif /* __LOG_ENTRY__ */
