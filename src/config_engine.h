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
#ifndef __CONFIG_ENGINE_H__
#define __CONFIG_ENGINE_H__

#include "config.h"

#ifdef HAVE_PYTHON
#include <Python.h>
#endif /* HAVE_PYTHON */

#ifdef HAVE_JS
#include <jsapi.h>
#endif

#ifdef HAVE_ICS
struct ics_server;
#endif /* HAVE_ICS */

#ifdef HAVE_XMPP
struct xmpp_server;
#endif /* HAVE_XMPP */

#ifdef HAVE_HTTP
struct http_server;
#endif /* HAVE_HTTP */

struct dcc_session;
struct network;
struct user;
struct tconfig_block;
struct t_crypto_module;
struct log_filter;
struct slist;

/* This is the main struct that information is loaded
 * into from the config block format.
 * All user contributed data goes in here.
 */
struct config
{
  struct network     *networks;
  struct dcc_session *dccs;
  struct user        *g_users;

#ifdef HAVE_HTTP
	struct http_server *http_servers;
#endif /* HAVE_HTTP */
#ifdef HAVE_ICS
	struct ics_server *ics_servers;
#endif /* HAVE_ICS */

#ifdef HAVE_XMPP
	struct xmpp_server *xmpp_servers;
#endif /* HAVE_XMPP */

  /* Unhandled tconfig blocks at toplevel */
  struct tconfig_block *tcfg;

	struct log_filter *filters;

	struct slist *tsockets;

	char *hash_type;

	char *crypto_name;
	struct t_crypto_module *crypto;

	char *log_file;
	char *dcc_motd;

  int fork; /* 0 to not fork, 1 to fork */
  int forked; /* Set to 1 after forking */
  
  int debug_level; /* see debug.h */

	int max_listeners;

#ifdef HAVE_PYTHON
  /* Global CLEAN python enviroment to be copied */
  PyObject *py_main;
  PyObject *py_main_dict;
	char **py_scripts;
	unsigned int py_scripts_size;
#endif /* HAVE_PYTHON */

#ifdef HAVE_PHP
	char **php_scripts;
	unsigned int	php_scripts_size;
#endif /* HAVE_PHP */

#ifdef HAVE_TCL
	char **tcl_scripts;
	unsigned int tcl_scripts_size;
#endif /* HAVE_TCL */

#ifdef HAVE_JS
  /* Runtime object for Spidermonkey */
  JSRuntime *js_rt;
	char **js_scripts;
	unsigned int js_scripts_size;
#endif /* HAVE_JS */
};

/* Function Prototypes */
int config_engine_init(char *filename);
struct config *config_engine_load(struct tconfig_block *tcfg);

#endif /* __CONFIG_ENGINE_H__ */
