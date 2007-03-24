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

  ret->pub       = NULL;
  ret->pub_head  = NULL;
  ret->pub_tail  = NULL;
  ret->pubm      = NULL;
  ret->pubm_head = NULL;
  ret->pubm_tail = NULL;
  ret->msg       = NULL;
  ret->msg_head  = NULL;
  ret->msg_tail  = NULL;
  ret->msgm      = NULL;
  ret->msgm_head = NULL;
  ret->msgm_tail = NULL;
  ret->join      = NULL;
  ret->join_head = NULL;
  ret->join_tail = NULL;
  ret->part      = NULL;
  ret->part_head = NULL;
  ret->part_tail = NULL;
  ret->quit      = NULL;
  ret->quit_head = NULL;
  ret->quit_tail = NULL;
  ret->notc      = NULL;
  ret->notc_head = NULL;
  ret->notc_tail = NULL;

  return ret;
}

