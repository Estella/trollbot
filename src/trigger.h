#ifndef __TRIGGER_H__
#define __TRIGGER_H__

#include "irc.h"

enum
{
  TRIG_PUB,
  TRIG_PUBM,
  TRIG_MSG,
  TRIG_MSGM,
  TRIG_PART,
  TRIG_NOTC,
  TRIG_JOIN,
  TRIG_QUIT
};

struct trigger
{
  char *glob_flags;
  char *chan_flags;

  int type;
 
  /* What triggers are matched against */
  char *mask;  

  void (*handler)(struct network *, struct trigger *, struct irc_data *);

  char *command;

  struct trigger *prev;
  struct trigger *next;  
};

struct trigger *new_trigger(char *flags, int type, char *mask, char *command, void (*handler)(struct network *, struct trigger *, struct irc_data *));
void trigger_match(struct network *net, struct irc_data *data);

#endif /* __TRIGGER_H__ */
