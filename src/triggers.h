/******************************
 * Trollbot                   *
 ******************************
 * Written by poutine DALnet  *
 ******************************
 * This software is public    *
 * domain. Free for any use   *
 * whatsoever.                *
 ******************************/

#ifndef __TRIGGERS_H__
#include "handlers.h"

struct triggers {
  int type;

  char *trigger;

  struct handlers *handler;

  char *exec;
  char *flags;
  
  struct triggers *prev, *next;
};

#define __TRIGGERS_H__
#endif /* __TRIGGERS_H__ */
