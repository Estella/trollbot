#ifndef __DEBUG_H__
#define __DEBUG_H__

struct log_filter;
struct log_entry;

enum {
  LOG_NONE  = 0,
  LOG_FATAL,
  LOG_ERROR,
  LOG_WARN,
  LOG_ALL,
  LOG_DEBUG
};

void console_log_filter_handler(struct log_filter *filter, struct log_entry *entry);
void troll_debug(int level, const char *fmt, ...);

#endif /* __DEBUG_H__ */
