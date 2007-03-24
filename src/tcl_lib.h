#ifndef __TCL_LIB_H__
#define __TCL_LIB_H__

#include <tcl.h>

/* Function prototypes */
int tcl_rand(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[]);
int tcl_puthelp(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[]);
int tcl_putserv(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[]);
int tcl_bind(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[]);

#endif /* __TCL_LIB_H__ */

