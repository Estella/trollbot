/*********************************************
 * TrollBot v1.0                             *
 *********************************************
 * TrollBot is an eggdrop-clone designed to  *
 * work with multiple networks and protocols *
 * in order to present a unified scriptable  *
 * event-based platform,                     *
 *********************************************
 * This software is PUBLIC DOMAIN. Feel free *
 * to use it for whatever use whatsoever.    *
 *********************************************
 * Originally written by poutine/DALnet      *
 *                       kicken/DALnet       *
 *                       comcor/DALnet       *
 *********************************************/
#ifndef __TCONFIG_H__

struct tconfig_block 
{
  char *key;
  char *value;
  
  struct tconfig_block *prev;
  struct tconfig_block *next;

  struct tconfig_block *parent;
  struct tconfig_block *child;
};

struct tconfig_block *tconfig_block_new(void);


/* I/O of tconfig struct */
struct tconfig_block *file_to_tconfig(const char *filename);
void tconfig_to_file(struct tconfig_block *tcfg, char *filename);

/* iterative now :) */
void free_tconfig(struct tconfig_block *tcfg);

/* Utility functions */

/* Searches subkeys (one level of depth) for search, if found returns value else NULL */
char *tconfig_get_subparam(struct tconfig_block *tcfg, const char *search);

void tconfig_foreach_length(struct tconfig_block *tcfg, int (*cback)(struct tconfig_block *));
void tconfig_foreach_child(struct tconfig_block *tcfg, int (*cback)(struct tconfig_block *));

/* Goes through the tree depth first calling callback function w/ depth each node */
void tconfig_foreach_depth_first(struct tconfig_block *tcfg, int (*cback)(struct tconfig_block *, int, void *), void *extra);

void tconfig_merge(struct tconfig_block *src, struct tconfig_block *dst);

struct tconfig_block *tconfig_isolate(struct tconfig_block *tcfg);

#define __TCONFIG_H__

#endif /* __TCONFIG_H__ */
