#ifndef __PYTHON_EMBED_H__
#define __PYTHON_EMBED_H__

#include <Python.h>

struct network;
struct config_engine;

void cfg_init_python(struct config_engine *cfg);
void net_init_python(struct config_engine *cfg, struct network *net);

#endif /* __PYTHON_EMBED_H__ */
