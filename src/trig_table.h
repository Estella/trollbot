#ifndef __TRIG_TABLE_H__
#define __TRIG_TABLE_H__

#include "trigger.h"

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
  struct trigger *quit;
  struct trigger *quit_head;
  struct trigger *quit_tail;
  struct trigger *notc;
  struct trigger *notc_head;
  struct trigger *notc_tail;
};

struct trig_table *new_trig_table(void);


#endif /* __TRIG_TABLE_H__ */
