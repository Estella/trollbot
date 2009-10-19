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
#ifndef __DEBUG_H__
#define __DEBUG_H__

struct network;
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

void console_log_filter_handler(struct network *net, struct log_filter *filter, struct log_entry *entry);
void troll_debug(int level, const char *fmt, ...);

#endif /* __DEBUG_H__ */
