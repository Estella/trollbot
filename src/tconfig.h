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
  char *identifier;
  char *type;
  char **data;
  
  struct tconfig_block *parent, *child;
  struct tconfig_block *next,   *prev;
};

struct global_config {
  struct network *networks;
  char **nicklist;
  int debug;
};


#define __TCONFIG_H__

#endif /* __TCONFIG_H__ */
