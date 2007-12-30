#include <Python.h>
#include "main.h"

#include "python_embed.h"
#include "python_lib.h"

#include "trigger.h"
#include "irc.h"
#include "network.h"
#include "server.h"
#include "channel.h"
#include "user.h"
#include "config_engine.h"
#include "python_lib.h"

#include "egg_lib.h"

/* Exported functions to Python */
PyMethodDef PyTbMethods[] = {

  {"bind", py_bind, METH_VARARGS,
   "binds an IRC event to python function"},

  {"putserv", py_putserv, METH_VARARGS,
   "Sends the text to the network."},

  {NULL, NULL, 0, NULL}
};

void cfg_init_python(struct config_engine *cfg)
{
  if (cfg->py_main != NULL)
    return;

  Py_Initialize();
  Py_InitModule("trollbot", PyTbMethods);

  cfg->py_main      = PyImport_AddModule("__main__");
  cfg->py_main_dict = PyModule_GetDict(cfg->py_main);
}


void net_init_python(struct config_engine *cfg, struct network *net)
{
  if (net->pydict != NULL || cfg->py_main_dict == NULL)
    return;

  net->pydict = PyDict_Copy(cfg->py_main_dict);
}

void py_handler(struct network *net, struct trigger *trig, struct irc_data *data, struct dcc_session *dcc, const char *dccbuf)
{
  
}
