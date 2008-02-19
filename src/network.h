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
struct t_timer;

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

enum network_status {
  NET_DISCONNECTED = 0,
	NET_INPROGRESS,
  NET_NONBLOCKCONNECT, /* A connect() call has been made, it is not in any fd set */
  NET_WAITINGCONNECT,  /* A connect() call has been made, it's now in a fd set    */
  NET_NOTREADY,        /* Socket has been accept()ed but not added to FD set      */
  NET_CONNECTED,
	NET_AUTHORIZED,
	NET_IDLE
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

	/* This settings makes the bot cycle forever through the server list until
 	 * it successfully connects to one.
 	 */ 
	int never_give_up;

  /* Time in seconds to wait before trying to reconnect */
  int connect_delay;

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


	/* Timers */
	struct t_timer *timers;

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
	unsigned int tcl_scripts_size;
#endif /* HAVE_TCL */  

#ifdef HAVE_PERL
	/* Copy over on rehash */
  struct interpreter *perlinterp;
#endif /* HAVE_PERL */

#ifdef HAVE_PYTHON
	/* Copy over on rehash */
  PyObject *py_netobj;
  PyObject *pydict;
  char **py_scripts;
  unsigned int py_scripts_size;
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
	unsigned int js_scripts_size;
#endif /* HAVE_JS */

  char *userfile;
  char *chanfile;
};

void network_connect(struct network *net);
void free_networks(struct network *net);

struct network *new_network(char *label);

#endif /* __NETWORK_H__ */
