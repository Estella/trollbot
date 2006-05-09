/******************************
 * Trollbot                   *
 ******************************
 * Written by poutine DALnet  *
 ******************************
 * This software is public    *
 * domain. Free for any use   *
 * whatsoever.                *
 ******************************/

#ifndef __USERS_H__

struct users {
  char *user;
  char *nick;
 
  char *ident;
  char *host;
  
  char *flags;

  int status;
};
 

#define __USERS_H__
#endif /* __USERS_H__ */
