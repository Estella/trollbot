#ifndef __NETWORK_H__
#define __NETWORK_H__

#include "config.h"

struct server;
struct channel;
struct user;
struct network;
struct trig_table;
struct tconfig_block;

#ifdef HAVE_TCL
#include <tcl.h>
#undef STRINGIFY
#undef JOIN
#undef END_EXTERN_C
#endif /* HAVE_TCL */

#ifdef HAVE_PERL
#include <EXTERN.h>
#include <perl.h>
#undef STRINGIFY
#undef JOIN
#undef END_EXTERN_C
#endif

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

  char *vhost;

  int status;

  /* Users */
  struct user *users;
  
  struct trig_table *trigs;  

  struct network *prev;
  struct network *next;

  struct tconfig_block *tindex;

#ifdef HAVE_TCL
  /* Network TCL Interpreter */
  Tcl_Interp *tclinterp;
#endif /* HAVE_TCL */  

#ifdef HAVE_PERL
  PerlInterpreter *perlinterp;
#endif /* HAVE_PERL */

  char *userfile;
  char *chanfile;
};

struct network *new_network(char *label);

#endif /* __NETWORK_H__ */
