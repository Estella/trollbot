#ifndef __TCL_LIB_H__
#define __TCL_LIB_H__

#include <tcl.h>
#undef STRINGIFY
#undef JOIN
#undef END_EXTERN_C

/* Function prototypes */
int tcl_isop(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[]);
int tcl_isvoice(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[]);


int tcl_getchanmode(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[]);
int tcl_encpass(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[]);
int tcl_topic(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[]);
int tcl_validuser(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[]);
int tcl_savechannels(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[]);
int tcl_finduser(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[]);
int tcl_utimer(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[]);
int tcl_timer(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[]);
int tcl_channels(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[]);
int tcl_save(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[]);
int tcl_botname(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[]);
int tcl_onchan(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[]);
int tcl_matchwild(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[]);
int tcl_rand(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[]);
int tcl_puthelp(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[]);
int tcl_putserv(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[]);
int tcl_bind(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[]);
int tcl_matchattr(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[]);
int tcl_countusers(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[]);
#endif /* __TCL_LIB_H__ */

