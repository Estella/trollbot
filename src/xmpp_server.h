#ifndef __XMPP_SERVER_H__
#define __XMPP_SERVER_H__

#include "config.h"

struct server;
struct tconfig_block;
struct log_filter;

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

enum xmpp_status {
  XMPP_DISCONNECTED = 0,
	XMPP_INPROGRESS,
  XMPP_NONBLOCKCONNECT, /* A connect() call has been made, it is not in any fd set */
  XMPP_WAITINGCONNECT,  /* A connect() call has been made, it's now in a fd set    */
  XMPP_NOTREADY,        /* Socket has been accept()ed but not added to FD set      */
  XMPP_CONNECTED,
	XMPP_AUTHORIZED,
	XMPP_IDLE
};

struct auth_type
{
	char *name;
	
	char *algo_name;

	struct auth_type *prev;
	struct auth_type *next;
};

/* All lists are at head */
struct xmpp_server
{
  char *label;

	/* I'm using some of the same stuff from the irc code
   * it should all be portable, or made to be portable
   * Interfaces need to be separated.
   */
	/* Copy over on rehash */
  struct server *cur_server;

	/* This should be used for inner "mirror" sites */
  struct server *servers;

	char *username;
	char *password_plain;
	char *password_md5;

	struct auth_type *auth_types_local;
	struct auth_type *auth_types_remote;

	/* Copy over on rehash */
  int sock;

	struct xmpp_trigger *xmpp_trigger_table;

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

	struct t_timer *timers;

	/* LOG FILTERS! */
	struct log_filter *filters;

  /* Unhandled blocks go here */
  struct tconfig_block *tcfg;

  struct xmpp_server *prev;
  struct xmpp_server *next;

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
};


/* Weak */
char *auth_type_get_algo_info(char *name);

struct auth_type *auth_type_add(struct auth_type *auth_types, struct auth_type *add);
struct auth_type *auth_type_del(struct auth_type *auth_types, struct auth_type *del);

void free_auth_types(struct auth_type *auth_types);
void free_auth_type(struct auth_type *at);

struct auth_type *new_auth_type(void);

struct xmpp_server *xmpp_server_from_tconfig_block(struct tconfig_block *tcfg);

struct xmpp_server *xmpp_server_add(struct xmpp_server *servers, struct xmpp_server *add);
struct xmpp_server *xmpp_server_del(struct xmpp_server *servers, struct xmpp_server *del);

void xmpp_server_connect(struct xmpp_server *xs);
void free_xmpp_servers(struct xmpp_server *servers);
void free_xmpp_server(struct xmpp_server *xs);

struct xmpp_server *new_xmpp_server(char *label);

#endif /* __XMPP_SERVER_H__ */
