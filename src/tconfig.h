/******************************
 * Trollbot                   *
 ******************************
 * Written by poutine DALnet  *
 ******************************
 * This software is public    *
 * domain. Free for any use   *
 * whatsoever.                *
 ******************************/
#ifndef __TCONFIG_H__

struct tconfig_block {
  char *key;
  char *value;
  
  struct tconfig_block *prev;
  struct tconfig_block *next;

  struct tconfig_block *parent;
  struct tconfig_block *child;
};


struct tconfig_block *parse_config(const char *filename);

#define __TCONFIG_H__

#endif /* __TCONFIG_H__ */
