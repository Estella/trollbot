#ifndef __LOG_ENTRY__
#define __LOG_ENTRY__

struct log_entry;

struct log_entry
{
	char *log_text;

	char *flags;

	struct log_entry *prev;
	struct log_entry *next;
};

void log_entry_printf(const char *flags, const char *fmt, ...);
void log_entry_free(struct log_entry *entry);
struct log_entry *log_entry_new(void);

#endif /* __LOG_ENTRY__ */
