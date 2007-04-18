#ifndef __NETWORK_H__
#define __NETWORK_H__

#include "config.h"

struct server;
struct channel;
struct user;
struct network;
struct trig_table;
struct tconfig_block;
struct dcc_session;

#include <time.h>

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

/* All lists are at head */
struct network
{
  char *label;

  struct server *cur_server;

  struct server *servers;

  struct channel *chans;
   
  int sock;

  /* This is the actual nick of the bot, nick is the wanted */
  char *botnick;
  
  char *nick;
  char *altnick;
  char *ident;
  char *realname;

  char *vhost;
  char *shost; /* Server given host */

  int status;
 
  /* This is to implement connection queueing 
   * connect_try is set to the amount of times 
   * try connecting before giving up, -1 if never
   */
  int connect_try;

  /* Time in seconds to wait before trying to reconnect */
  time_t connect_delay;

  /* if (connect_try--) if (last_try + connect_delay <= time(NULL)) connect() */
  time_t last_try;

  /* Users */
  struct user *users;
 
  /* DCCs */
  struct dcc_session *dccs;
 
  /* Each network has its own DCC listener */
  int dcc_listener;

  struct trig_table *trigs;  

  struct network *prev;
  struct network *next;

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

void free_networks(struct network *net);

struct network *new_network(char *label);

#endif /* __NETWORK_H__ */
