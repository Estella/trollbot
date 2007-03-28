#ifndef __NETWORK_H__
#define __NETWORK_H__

#include <string.h>

#include "config.h"

#ifdef HAVE_TCL
#include <tcl.h>
#endif /* HAVE_TCL */


enum
{
  STATUS_IGNORED      = -1,
  STATUS_DISCONNECTED =  0,
  STATUS_CONNECTED    =  1,
  STATUS_AUTHORIZED,
  STATUS_IDLE
};

struct network
{
  char *label;

  struct server *cur_server;
  struct server *server_head;
  struct server *server_list;
  struct server *server_tail;

  struct channel *channel_list;
  struct channel *channel_head;
  struct channel *channel_tail;
   
  int sock;
  
  char *nick;
  char *altnick;
  char *ident;
  char *realname;

  int status;

  /* Users */
  struct user *users;
  struct user *users_head;
  struct user *users_tail;
  
  struct trig_table *trigs;  

  struct network *prev;
  struct network *next;

#ifdef HAVE_TCL
  /* Network TCL Interpreter */
  Tcl_Interp *tclinterp;
#endif /* HAVE_TCL */  
};

struct network *new_network(char *label);

#endif /* __NETWORK_H__ */
