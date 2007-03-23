#ifndef __TRIG_TABLE_H__
#define __TRIG_TABLE_H__

#include "trigger.h"

struct trig_table
{
  /* All point to head */
  struct trigger *pub;
  struct trigger *pubm;
  struct trigger *msg;
  struct trigger *msgm;
  struct trigger *part;
  struct trigger *join;
  struct trigger *quit;
  struct trigger *notc;
};

struct trig_table *new_trig_table(void);


#endif /* __TRIG_TABLE_H__ */
