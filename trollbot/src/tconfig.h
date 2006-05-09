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

struct glob_config {
  char *nick;
  char *altnick;

  char *ident;
  char *realname;

  char *vhost;

  int fork;
  int debug;

  struct servers *server;

  int sock;
};
    

#define __TCONFIG_H__

#endif /* __TCONFIG_H__ */
