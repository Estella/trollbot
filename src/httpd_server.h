#ifndef __HTTPD_SERVER_H__
#define __HTTPD_SERVER_H__

#include "config.h"

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

#define HTTPD_MAX 100

enum httpd_status {
	HTTPD_UNINITIALIZED = 0,
	HTTPD_LISTENING
};

/* All lists are at head */
struct httpd_server
{
  char *label;

	char *username;
	char *password;

	/* Copy over on rehash */
  int sock;

/* struct httpd_trigger_table *httpd_trigger_table; */

  char *host;
	int port;

	/* Copy over on rehash */
  int status;

	char *web_root;

	struct t_timer *timers;

	/* LOG FILTERS! */
	struct log_filter *filters;

  /* Unhandled blocks go here */
  struct tconfig_block *tcfg;

  struct httpd_server *prev;
  struct httpd_server *next;

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


struct httpd_server *httpd_server_from_tconfig_block(struct tconfig_block *tcfg);

struct httpd_server *httpd_server_add(struct httpd_server *servers, struct httpd_server *add);
struct httpd_server *httpd_server_del(struct httpd_server *servers, struct httpd_server *del);

void httpd_server_listen(struct httpd_server *httpd);
void free_httpd_servers(struct httpd_server *httpd_servers);
void free_httpd_server(struct httpd_server *httpd);

struct httpd_server *new_httpd_server(char *label);

#endif /* __HTTPD_SERVER_H__ */
