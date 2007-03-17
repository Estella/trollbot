#ifndef __DEBUG_H__
#define __DEBUG_H__

enum {
  LOG_NONE  = 0,
  LOG_FATAL,
  LOG_WARN,
  LOG_ALL,
  LOG_DEBUG
};

void troll_debug(int level, const char *fmt, ...);

#endif /* __DEBUG_H__ */
