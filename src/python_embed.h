#ifndef __PYTHON_EMBED_H__
#define __PYTHON_EMBED_H__

#include <Python.h>
#include <config_engine.h>
#include "irc.h"
#include "trigger.h"

struct network;
struct config_engine;

void cfg_init_python(struct config *cfg);
void net_init_python(struct config *cfg, struct network *net);
void python_load_scripts_from_config(struct config *cfg);
void load_python_module(char*);
void load_python_net_module(struct network *net, char * filename);
void py_handler(struct network *net, struct trigger *trig, struct irc_data *data, struct dcc_session *dcc, const char *dccbuf);
PyObject * call_python_method(char * module, char * method, PyObject ** args, int numargs);
void python_add_path(char * pathname);
#endif /* __PYTHON_EMBED_H__ */
