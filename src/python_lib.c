#include "main.h"
#include "python_lib.h"

#include <Python.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "config_engine.h"
#include "python_embed.h"
#include "trigger.h"
#include "network.h"
#include "egg_lib.h"
#include "irc.h"
#include "user.h"

  
PyObject *py_bind(PyObject *self, PyObject *args) {

  struct network *net;
  PyObject *network;

  char *netw;
  char *type;
  char *flags;
  char *keyword;
  char *proc;

  /* Nick uhost hand chan arg */
  if (!PyArg_ParseTuple(args, "Ossss", &network,&type, &flags, &keyword, &proc)) {
    troll_debug(LOG_DEBUG, "[python-bindings] py_bind failed to parse arguments from script");
    Py_RETURN_FALSE;
  }

   
  net = (struct network *) PyCObject_AsVoidPtr(network); 

  if (net == NULL) {
    troll_debug(LOG_DEBUG, "[python-bindings] py_bind failed to resolve network object reference");
    Py_RETURN_FALSE;
  }
  //give it to egglib
  if (!egg_bind(net, type, flags, keyword, proc, py_handler)) {
    troll_debug(LOG_DEBUG, "[python-bindings] py_bind; egg_bind failed: (%s, %s, %s, %s, %s)", net, type, flags, keyword, proc);
    Py_RETURN_FALSE;
  }
  troll_debug(LOG_DEBUG, "[python-bindings] py_bind: bound trigger `%s' to method `%s'", keyword, proc);
  Py_RETURN_TRUE;
}

PyObject *py_putserv(PyObject *self, PyObject *args) {

  struct network *net;
  PyObject *netw;
  char * msg;

  if (!PyArg_ParseTuple(args, "Os", &netw,&msg)) {
     troll_debug(LOG_DEBUG, "[python-bindings] py_putserv failed to parse arguments from script");
     Py_RETURN_FALSE;
  }
  
  net = (struct network*)PyCObject_AsVoidPtr(netw);

  if (net == NULL) {
    troll_debug(LOG_DEBUG, "[python-bindings] py_putserv Failed to resolve network for object reference");
    Py_RETURN_FALSE;
  }

  irc_printf(net->sock, msg);

  Py_RETURN_TRUE;
}

/* vim: tabstop=2
 */
