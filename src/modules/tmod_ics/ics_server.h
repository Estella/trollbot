#ifndef __ICS_SERVER_H__
#define __ICS_SERVER_H__

#include "config.h"

struct server;
struct tconfig_block;
struct log_filter;
struct ics_game;
struct tsocket;

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

enum ics_status {
  ICS_DISCONNECTED = 0,
	ICS_INPROGRESS,
  ICS_NONBLOCKCONNECT, /* A connect() call has been made, it is not in any fd set */
  ICS_WAITINGCONNECT,  /* A connect() call has been made, it's now in a fd set    */
  ICS_NOTREADY,        /* Socket has been accept()ed but not added to FD set      */
  ICS_CONNECTED,
	ICS_AUTHORIZED,
	ICS_IDLE
};


/* All lists are at head */
struct ics_server
{
  char *label;

	/* WTF is this */
	/* I'm using some of the same stuff from the irc code
   * it should all be portable, or made to be portable
   * Interfaces need to be separated.
   */
	/* Copy over on rehash */
  struct server *cur_server;

	/* This should be used for inner "mirror" sites */
  struct server *ics_servers;

	char *username;
	char *password;

	/* Copy over on rehash */
  struct tsocket *tsock;

	struct ics_trigger_table *ics_trigger_table;

  char *vhost;
  char *shost; /* Server given host */

	/* Copy over on rehash */
  int status;

	/* This is what FICS recognizes me as */
	char *my_name; 

	/* This settings makes the bot cycle forever through the server list until
 	 * it successfully connects to one.
 	 */ 
	int never_give_up;

	/* Stupid thing I use for the on connect trigger */
	int connected;

  /* Time in seconds to wait before trying to reconnect */
  int connect_delay;

  /* if (connect_try--) if (last_try + connect_delay <= time(NULL)) connect() */
  time_t last_try;

	struct t_timer *timers;

	/* LOG FILTERS! */
	struct log_filter *filters;

  /* Unhandled blocks go here */
  struct tconfig_block *tcfg;

	struct ics_game *game;

  struct ics_server *prev;
  struct ics_server *next;

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


struct ics_server *ics_server_from_tconfig_block(struct tconfig_block *tcfg);

struct ics_server *ics_server_add(struct ics_server *servers, struct ics_server *add);
struct ics_server *ics_server_del(struct ics_server *servers, struct ics_server *del);

void ics_server_connect(struct ics_server *ics);
void free_ics_servers(struct ics_server *ics_servers);
void free_ics_server(struct ics_server *ics);

struct ics_server *new_ics_server(char *label);

#endif /* __ICS_SERVER_H__ */
