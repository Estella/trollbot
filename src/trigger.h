#ifndef __TRIGGER_H__
#define __TRIGGER_H__

#include "irc.h"

struct trigger
{
  char *glob_flags;
  char *chan_flags;
 
  /* What triggers are matched against */
  char *mask;  

  void (*handler)(struct trigger *, struct irc_data *);

  char *command;

  struct trigger *prev;
  struct trigger *next;  
};

struct trigger *new_trigger(char *flags, char *mask, char *command, void (*handler)(struct trigger *, struct irc_data *));
void trigger_match(struct network *net, struct irc_data *data);

#endif /* __TRIGGER_H__ */
