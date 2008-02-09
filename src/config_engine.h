#ifndef __CONFIG_ENGINE_H__
#define __CONFIG_ENGINE_H__

#include "config.h"

#ifdef HAVE_PYTHON
#include <Python.h>
#endif /* HAVE_PYTHON */

#ifdef HAVE_JS
#include <jsapi.h>
#endif

struct dcc_session;
struct network;
struct user;
struct tconfig_block;

/* This is the main struct that information is loaded
 * into from the config block format.
 * All user contributed data goes in here.
 */
struct config
{
  struct network     *networks;
  struct dcc_session *dccs;
  struct user        *g_users;

  /* Unhandled tconfig blocks at toplevel */
  struct tconfig_block *tcfg;

  int fork; /* 0 to not fork, 1 to fork */
  int forked; /* Set to 1 after forking */
  
  int debug_level; /* see debug.h */

#ifdef HAVE_PYTHON
  /* Global CLEAN python enviroment to be copied */
  PyObject *py_main;
  PyObject *py_main_dict;
#endif /* HAVE_PYTHON */

#ifdef HAVE_JS
  /* Runtime object for Spidermonkey */
  JSRuntime *js_rt;
#endif
};

/* Function Prototypes */
int config_engine_init(char *filename);
struct config *config_engine_load(struct tconfig_block *tcfg);

#endif /* __CONFIG_ENGINE_H__ */
