#ifndef __TRIGGER_H__
#define __TRIGGER_H__

struct trigger
{
  char *glob_flags;
  char *chan_flags;
 
  /* What triggers are matched against */
  char *mask;  

  void (*handler)(struct trigger *);

  char *command;

  struct trigger *prev;
  struct trigger *next;  
};

struct trigger *new_trigger(char *flags, char *mask, char *command, void (*handler)(struct trigger *));

#endif /* __TRIGGER_H__ */
