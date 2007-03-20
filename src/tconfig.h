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


struct tconfig_block *file_to_tconfig(const char *filename);
void free_tconfig_r(struct tconfig_block *tcfg);
void free_tconfig(struct tconfig_block *tcfg);


#define __TCONFIG_H__

#endif /* __TCONFIG_H__ */
