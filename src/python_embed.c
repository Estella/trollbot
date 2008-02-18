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

#define PY_INTERNAL_CORE 1

#ifdef PY_INTERNAL_CORE
#define PY_CORE_LIB "__main__"
#else
#define PY_CORE_LIB "tb_py_core"
#endif
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
 
#ifdef PY_INTERNAL_CORE
  int ret;
  ret = PyRun_SimpleString(python_core_module_code);
  if (!ret) {
    troll_debug(LOG_DEBUG, "[python] Loaded internal core module");
  } else {
    troll_debug(LOG_DEBUG, "[python] An error occurred while loading internal core module");
  }
#else
  load_python_module(PY_CORE_LIB);
#endif
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
    net = net->next;
  }
}

/**
 * loads a python module for the provided network
 */
void load_python_net_module(struct network *net, char * filename) {

  PyObject * args[2];
  PyObject *rval;
   
  troll_debug(LOG_DEBUG, "[python] loading module `%s' for network %s", filename, net->label);
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

#ifdef PY_INTERNAL_CORE
char * python_core_module_code = 
"import imp\n"
"import trollbot\n"
"import sys\n"
"\n"
"NETWORKS = {}\n"
"\n"
"#TrollbotNetworkInterface\n"
"#Marshals access to the trollbot module for the given network.\n"
"#Each script loaded will have a reference to an instance of this\n"
"#object for the network it is attached to\n"
"\n"
"class TrollbotNetworkInterface:\n"
"\n"
"   def __init__(self, network):\n"
"      self.network = network #ptr to TB net struct\n"
"      self.modules = {}\n"
"   #end __init__\n"
"\n"
"   def load_module(self, name):\n"
"      ok = False \n"
"      if (name.find(\"/\") != -1):\n"
"         comps = name.split(\"/\")\n"
"         path = \"/\".join(comps[:-1])\n"
"         name = comps[-1]\n"
"         if path not in sys.path:\n"
"            sys.path.insert(0,path)\n"
"      #end path handling\n"
"\n"
"      #strip .py off\n"
"      if (name[-3:] == \".py\"):\n"
"         name = name[:-3]\n"
"\n"
"      fp, pathname, description = imp.find_module(name)\n"
"\n"
"      try:\n"
"         module = imp.load_module(name, fp, pathname, description)\n"
"         if module is not None:\n"
"            #pass module a reference to us\n"
"            module.trollbot = self\n"
"            #initialize our module\n"
"            if getattr(module, \"load\") != None:\n"
"               module.load()\n"
"            else:\n"
"               raise \"Missing load() method for module!\"\n"
"            #save the module for access later\n"
"            self.modules[name] = module\n"
"            ok = True\n"
"      finally:\n"
"         if fp:\n"
"            fp.close\n"
"      return ok\n"
"   #end load_module\n"
"\n"
"   def bind(self,*args):\n"
"      trollbot.bind(self.network, *args)\n"
"\n"
"   def putserv(self, *args):\n"
"      trollbot.putserv(self.network, *args)\n"
"\n"
"   def call_method(self,callback, *args):\n"
"      #get module name\n"
"      (module, method) = callback.split(\".\")\n"
"      if self.modules.has_key(module):\n"
"         cb = getattr(self.modules[module], method)\n"
"         cb(*args)\n"
"\n"
"#end TrollbotNetworkInterface\n"
"\n"
"\n"
"##\n"
"## INTERNAL FUNCTIONS\n"
"##\n"
"## Please don't call these unless you know what you are doing.\n"
"## These are meant to be called through the Python C/API\n"
"##\n"
"\n"
"def __TB_init_network(network):\n"
"   if not NETWORKS.has_key(network):\n"
"      NETWORKS[network] = TrollbotNetworkInterface(network)\n"
"#end init_network\n"
"\n"
"def __TB_load_module(network, module):\n"
"   if NETWORKS.has_key(network):\n"
"      return NETWORKS[network].load_module(module)\n"
"   #end if\n"
"   return False\n"
"#end __TB_load_module\n"
"\n"
"def __TB_call_method(network, callback, *args):\n"
"   if NETWORKS.has_key(network):\n"
"      return NETWORKS[network].call_method(callback, *args)\n"
"   return False\n"
"#end __TB_call_method\n"
"\n"
"def __TB_add_path(path):\n"
"   sys.path.append(path)\n"
"#end __TB_add_path\n";
#endif

/* vim: tabstop=2
 */
