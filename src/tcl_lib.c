#ifdef HAVE_TCL
#include <stdio.h>
#include <tcl.h>

#include "main.h"
#include "irc.h"
#include "channels.h"
#include "servers.h"
#include "scripts.h"



/* puthelp <message>
 * uses the queue
 */
int tcl_puthelp(ClientData clientData,
                Tcl_Interp *interp,
                int objc,
                Tcl_Obj *const objv[])
{
  if (objc != 2)
  {
    Tcl_WrongNumArgs(interp,objc,objv,"puthelp message");
    return TCL_ERROR;
  }

  irc_printf(glob_server_head->sock,"%s",Tcl_GetString(objv[1]));

  return TCL_OK;
}


/* putserv <message> 
 * bypasses the queue
 */
int tcl_putserv(ClientData clientData,
                Tcl_Interp *interp,
                int objc,
                Tcl_Obj *const objv[])
{
  if (objc != 2)
  {
    Tcl_WrongNumArgs(interp,objc,objv,"putserv message");
    return TCL_ERROR;
  }

  irc_printf(glob_server_head->sock,"%s",Tcl_GetString(objv[1]));

  return TCL_OK;
}

/* bind <type> <flags> <keyword/mask> [proc-name] */
int tcl_bind(ClientData clientData, 
             Tcl_Interp *interp, 
             int objc, 
             Tcl_Obj *const objv[]) 
{
  int type = 0;

  if (objc != 5) 
  {
    Tcl_WrongNumArgs(interp,objc,objv,"bind type flags keyword proc");
    return TCL_ERROR;
  }
 
  if (!strcmp(Tcl_GetString(objv[1]),"pub"))
    type = PUB;
  else if (!strcmp(Tcl_GetString(objv[1]),"pubm"))
    type = PUBM;
  else if (!strcmp(Tcl_GetString(objv[1]),"msg"))
    type = MSG;
  else if (!strcmp(Tcl_GetString(objv[1]),"msgm"))
    type = MSGM;
  else if (!strcmp(Tcl_GetString(objv[1]),"join"))
    type = JOIN;
  else if (!strcmp(Tcl_GetString(objv[1]),"part"))
    type = PART;
  else if (!strcmp(Tcl_GetString(objv[1]),"quit"))
    type = QUIT;
  else if (!strcmp(Tcl_GetString(objv[1]),"mode"))
    type = MODE;
  else
    return TCL_ERROR;

  /* Flag Parsing */
  /* Skipped */

  add_trigger(type,
              Tcl_GetString(objv[3]),
              "tcl-handle",
              NULL,
              Tcl_GetString(objv[4]));
   
  return TCL_OK; 
}

#endif /* HAVE_TCL */
