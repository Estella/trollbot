#include <Python.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "network.h"
#include "egg_lib.h"
#include "python_lib.h"
#include "user.h"
#include "config_engine.h"

PyObject *py_bind(PyObject *self, PyObject *args)
{
  char *type;
  char *flags;
  char *keyword;
  char *proc;

  /* Nick uhost hand chan arg */
  if (!PyArg_ParseTuple(args, "ssss", &type, &flags, &keyword, &proc))
    return NULL;

  
    
  
}

PyObject *py_putserv(PyObject *self, PyObject *args)
{

}


