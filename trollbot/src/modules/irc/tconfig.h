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

#define BUFFER_SIZE 1024

struct tconfig_block {
  char *key;
  char *value;
  
  struct tconfig_block *prev;
  struct tconfig_block *next;

  struct tconfig_block *parent;
  struct tconfig_block *child;
};

/* I/O of tconfig struct */
struct tconfig_block *file_to_tconfig(const char *filename);
void tconfig_to_file(struct tconfig_block *tcfg, char *filename);

/* iterative now :) */
void free_tconfig(struct tconfig_block *tcfg);

/* Utility functions */

/* Searches subkeys (one level of depth) for search, if found returns value else NULL */
char *tconfig_get_subparam(struct tconfig_block *tcfg, const char *search);

void tconfig_foreach_width(struct tconfig_block *tcfg, int (*cback)(struct tconfig_block *, void *), void *extra);
void tconfig_foreach_child(struct tconfig_block *tcfg, int (*cback)(struct tconfig_block *));

/* Goes through the tree depth first calling callback function w/ depth each node */
void tconfig_foreach_depth_first(struct tconfig_block *tcfg, int (*cback)(struct tconfig_block *, int, void *), void *extra);

void tconfig_merge(struct tconfig_block *src, struct tconfig_block *dst);

struct tconfig_block *tconfig_isolate(struct tconfig_block *tcfg);

void tconfig_final_pass(struct tconfig_block *tcfg);

#define __TCONFIG_H__

#endif /* __TCONFIG_H__ */
