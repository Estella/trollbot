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
struct interpreter;
#endif /* HAVE_PERL */

#ifdef HAVE_PYTHON
#include <Python.h>
#endif /* HAVE_PYTHON */

#ifdef HAVE_JS
#include <jsapi.h>
#endif /* HAVE_JS */

enum network_status
{
  STATUS_IGNORED      = -1,
  STATUS_DISCONNECTED =  0,
  STATUS_NOTREADY     =  1,
  STATUS_WANTDISC,
  STATUS_CONNECTED,
  STATUS_AUTHORIZED,
  STATUS_IDLE
};

/* All lists are at head */
struct network
{
  char *label;

	/* Copy over on rehash */
  struct server *cur_server;

  struct server *servers;

	/* Copy over on rehash */
  struct channel *chans;
   
	/* Copy over on rehash */
  int sock;

  /* This is the actual nick of the bot, nick is the wanted */
  char *botnick;
  
  char *nick;
  char *altnick;
  char *ident;
  char *realname;

  char *vhost;
  char *shost; /* Server given host */

	/* Copy over on rehash */
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
	/* Copy over on rehash */
  struct user *users;

	/* For Compatibility with eggdrop */
	int handlen;
 
  /* DCCs */
	/* Copy over on rehash */
  struct dcc_session *dccs;
 
  /* Each network has its own DCC listener */
	/* Copy over on rehash */
  int dcc_listener;

  int dcc_port;

	/* Copy over on rehash */
  struct trig_table *trigs;  

  /* Unhandled blocks go here */
  struct tconfig_block *tcfg;

  struct network *prev;
  struct network *next;

#ifdef HAVE_TCL
  /* Network TCL Interpreter */
	/* Copy over on rehash */
  Tcl_Interp *tclinterp;

	char **tcl_scripts;
	int tcl_scripts_size;
#endif /* HAVE_TCL */  

#ifdef HAVE_PERL
	/* Copy over on rehash */
  struct interpreter *perlinterp;
#endif /* HAVE_PERL */

#ifdef HAVE_PYTHON
	/* Copy over on rehash */
  PyObject *py_netobj;
  PyObject *pydict;
  char **py_paths;
  int py_paths_size;
  char **py_scripts;
  int py_scripts_size;
#endif /* HAVE_PYTHON */

#ifdef HAVE_JS
	/* Copy both over on rehash */
	JSContext *cx;
	JSObject  *global;
	
	/* HACK */
	/* Copy both over on rehash */
	JSContext *plain_cx;
	JSObject  *plain_global;

	char **js_scripts;
	int js_scripts_size;
#endif /* HAVE_JS */

  char *userfile;
  char *chanfile;
};

void free_networks(struct network *net);

struct network *new_network(char *label);

#endif /* __NETWORK_H__ */
