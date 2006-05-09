/******************************
 * Trollbot                   *
 ******************************
 * Written by poutine DALnet  *
 ******************************
 * This software is public    *
 * domain. Free for any use   *
 * whatsoever.                *
 ******************************/
#ifndef __HANDLERS_H__

#define HANDLER_DISABLED -1
#define HANDLER_SHUTDOWN  0
#define HANDLER_READY     1

struct handlers {
  char *name;
  int (*startup_func)(void);
  int (*exec_func)(struct irc_data *,const char *);
  int (*shutdown_func)(void);

  int persistant;
  int usecount;

  int status;

  struct handlers *prev, *next;
};


#define __HANDLERS_H__
#endif /* __HANDLERS_H__ */
