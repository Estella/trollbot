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
#ifndef __HTTP_SERVER_H__
#define __HTTP_SERVER_H__

#include "config.h"

struct tsocket;
struct http_request;
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

#define HTTP_MAX 100

/* All lists are at head */
struct http_server
{
  char *label;

	char *username;
	char *password;

	/* Copy over on rehash */
  int sock;
	struct tsocket *tsock;

/* struct http_trigger_table *http_trigger_table; */

  char *host;
	int port;

	char *web_root;

	struct t_timer *timers;

	/* LOG FILTERS! */
	struct log_filter *filters;

  /* Unhandled blocks go here */
  struct tconfig_block *tcfg;

	struct http_request *requests;

  struct http_server *prev;
  struct http_server *next;

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


int http_server_accept(struct tsocket *tsock);

struct http_server *http_server_from_tconfig_block(struct tconfig_block *tcfg);

struct http_server *http_server_add(struct http_server *servers, struct http_server *add);
struct http_server *http_server_del(struct http_server *servers, struct http_server *del);

void http_server_listen(struct http_server *http);
void free_http_servers(struct http_server *http_servers);
void free_http_server(struct http_server *http);

struct http_server *new_http_server(char *label);

#endif /* __HTTP_SERVER_H__ */
