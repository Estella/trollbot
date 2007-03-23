#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "trig_table.h"
#include "trigger.h"
#include "util.h"

struct trig_table *new_trig_table(void)
{
  struct trig_table *ret;

  ret = tmalloc(sizeof(struct trig_table));

  ret->pub  = NULL;
  ret->pubm = NULL;
  ret->msg  = NULL;
  ret->msgm = NULL;
  ret->join = NULL;
  ret->part = NULL;
  ret->quit = NULL;
  ret->notc = NULL;

  return ret;
}

