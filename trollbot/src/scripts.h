/******************************
 * Trollbot                   *
 ******************************
 * Written by poutine DALnet  *
 ******************************
 * This software is public    *
 * domain. Free for any use   *
 * whatsoever.                *
 ******************************/
#ifndef __SCRIPTS_H__

struct scripts {
  char *filename;

  int status;

  struct scripts *prev, *next;
};

#define __SCRIPTS_H__
#endif /* __SCRIPTS_H__ */
