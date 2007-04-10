#ifndef __TRIGGER_H__
#define __TRIGGER_H__

struct network;
struct irc_data;

enum
{
  TRIG_PUB,
  TRIG_PUBM,
  TRIG_MSG,
  TRIG_MSGM,
  TRIG_PART,
  TRIG_NOTC,
  TRIG_JOIN,
  TRIG_SIGN,
  TRIG_KICK
};

struct trig_table
{
  /* All point to head */
  struct trigger *pub;
  struct trigger *pub_head;
  struct trigger *pub_tail;
  struct trigger *pubm;
  struct trigger *pubm_head;
  struct trigger *pubm_tail;
  struct trigger *msg;
  struct trigger *msg_head;
  struct trigger *msg_tail;
  struct trigger *msgm;
  struct trigger *msgm_head;
  struct trigger *msgm_tail;
  struct trigger *part;
  struct trigger *part_head;
  struct trigger *part_tail;
  struct trigger *join;
  struct trigger *join_head;
  struct trigger *join_tail;
  struct trigger *sign;
  struct trigger *sign_head;
  struct trigger *sign_tail;
  struct trigger *kick;
  struct trigger *kick_head;
  struct trigger *kick_tail;
  struct trigger *notc;
  struct trigger *notc_head;
  struct trigger *notc_tail;
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
struct trig_table *new_trig_table(void);

#endif /* __TRIGGER_H__ */
