#include <Python.h>
#include "main.h"
#include "python_embed.h"

#include "trigger.h"
#include "irc.h"
#include "network.h"
#include "server.h"
#include "channel.h"
#include "user.h"
#include "config_engine.h"
#include "python_lib.h"

#include "egg_lib.h"

#define PY_CORE_LIB "tb_py_core"
#define PY_NET_INIT_METH "__TB_init_network"
#define PY_NET_LOAD_MODULE_METH "__TB_load_module"
#define PY_NET_CALL_METH "__TB_call_method"
#define PY_NET_ADD_PATH_METH "__TB_add_path"

/* Exported functions to Python */
PyMethodDef PyTbMethods[] = {

  {"bind", py_bind, METH_VARARGS,
   "binds an IRC event to python function"},

  {"putserv", py_putserv, METH_VARARGS,
   "Sends the text to the network."},

  {NULL, NULL, 0, NULL}
};

void cfg_init_python(struct config *cfg) {
  if (cfg->py_main != NULL)
    return;

  //initialize the python interpreter
  Py_Initialize();
  troll_debug(LOG_DEBUG, "[python] Initialized interpreter");
  //initialize the module which exposes to python
  //our wrappers for the bots functionality we want to expose
  //NOTE: all exposables need to be defined in the PyTbMethods
  Py_InitModule("trollbot", PyTbMethods);
  troll_debug(LOG_DEBUG, "[python] Loaded trollbot binding module");
  
  load_python_module(PY_CORE_LIB);
}


void net_init_python(struct config *cfg, struct network *net) {

  PyObject * args[1];
  PyObject * rval;

  troll_debug(LOG_DEBUG, "[python] initializing network context %s",net->label);

  net->py_netobj = PyCObject_FromVoidPtr((void*)net, NULL);
  args[0] = net->py_netobj;

  rval = call_python_method(PY_CORE_LIB, PY_NET_INIT_METH, args, 1);
}

void python_load_scripts_from_config(struct config *cfg) {

  int i;
  struct network *net = cfg->networks;

  while (net != NULL) {
    if (net->py_scripts != NULL) {
      for (i=0; net->py_scripts[i] != NULL; i++) {
        load_python_net_module(net, net->py_scripts[i]);
      }
    }
    if (net->py_paths != NULL) {
      for (i = 0; net->py_paths[i] != NULL; i++) {
        python_add_path(net->py_paths[i]);
      }
    }
    net = net->next;
  }
}

/**
 * loads a python module for the provided network
 */
void load_python_net_module(struct network *net, char * filename) {

  PyObject * args[2];
  PyObject *rval;

  args[0] = net->py_netobj;
  args[1] = PyString_FromString(filename);

  rval = call_python_method(PY_CORE_LIB, PY_NET_LOAD_MODULE_METH, args, 2);
}

/**
 * load a python module from its module name
 */
void load_python_module(char *filename) {

  PyObject *pyModuleName;
  PyObject *pyModule;

  pyModuleName = PyString_FromString(filename);
  pyModule = PyImport_Import(pyModuleName);
  if (pyModule != NULL) {
    troll_debug(LOG_DEBUG, "[python] Core Module (%s) loaded", filename);
    //Py_DECREF(pyModule); 
  } else {
    if (PyErr_Occurred()) 
      PyErr_Print();
    troll_debug(LOG_DEBUG, "[python] Failed to load Core Module (%s)", filename);
  }
  Py_DECREF(pyModuleName);
}

/**
 * Execute a python callback on the given network
 */
void py_handler(struct network *net, struct trigger *trig, struct irc_data *data, struct dcc_session *dcc, const char *dccbuf) {
  PyObject *args[8];
  PyObject *rval;

  troll_debug(LOG_DEBUG, "[python] py_handler called for command %s", trig->command);
  switch (trig->type) {
    case TRIG_PUB:
      args[0] = net->py_netobj;
      args[1] = PyString_FromString(trig->command);
      args[2] = PyString_FromString(net->label);
      args[3] = PyString_FromString(data->prefix->nick);
      args[4] = PyString_FromString(data->prefix->host);
      args[5] = PyString_FromString("*");
      args[6] = PyString_FromString(data->c_params[0]);
      args[7] = PyString_FromString(egg_makearg(data->rest_str, trig->mask));

      rval = call_python_method(PY_CORE_LIB, PY_NET_CALL_METH, args, 8);
      break;   
  }
}


PyObject * call_python_method(char * module, char * method, PyObject ** args, int numargs) {

  int i;
  PyObject * module_py;
  PyObject * method_py;
  PyObject * args_py;
  PyObject * rval_py;

  /* get reference to module containing method */
  module_py = PyImport_AddModule(module);
  if (module_py == NULL) {
    troll_debug(LOG_DEBUG, "[python] Could not locate module: %s", module);
    Py_RETURN_FALSE;
  }

  /* get reference to method in module */
  method_py = PyObject_GetAttrString(module_py, method);
  if (method_py == NULL) {
    troll_debug(LOG_DEBUG, "[python] Could not locate method %s in module %s",module, method);
    Py_RETURN_FALSE;
  }

  if (PyCallable_Check(method_py)) {
    /* create argument tuple */
    args_py = PyTuple_New(numargs);
    for (i = 0; i < numargs; i++) {
      PyTuple_SetItem(args_py, i, args[i]);
    }

    rval_py = PyObject_CallObject(method_py, args_py);

    Py_DECREF(args_py);
    Py_DECREF(method_py);
    //Py_DECREF(module_py);

    if (rval_py == NULL) {
      troll_debug(LOG_DEBUG, "[python] an error occurred calling %s.%s()", module, method);
      if (PyErr_Occurred()) {
        PyErr_Print();
      }
      Py_RETURN_FALSE;
    } else {
      return rval_py;
    }

  } else {
    troll_debug(LOG_DEBUG, "[python] %s.%s is not callable", method, module);
    Py_RETURN_FALSE;
  }
}

/**
 * adds a string path to the interpreter's sys.path list
 */
void python_add_path(char * pathname) {
   PyObject * args[1];
   PyObject * rval;

   rval = call_python_method(PY_CORE_LIB, PY_NET_ADD_PATH_METH, args, 1);
}

/* vim: tabstop=2
 */
