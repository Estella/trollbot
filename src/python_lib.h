#ifndef __PYTHON_LIB_H__
#define __PYTHON_LIB_H__

#include <Python.h>


PyObject *py_savechannels(PyObject *self, PyObject *args);
PyObject *py_validuser(PyObject *self, PyObject *args);
PyObject *py_bind(PyObject *self, PyObject *args);
PyObject *py_putserv(PyObject *self, PyObject *args);
PyObject *py_troll_debug(PyObject*self, PyObject *args);
PyObject *py_privmsg(PyObject *self, PyObject *args);


#endif /* __PYTHON_LIB_H__ */
