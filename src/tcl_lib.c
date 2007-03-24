/* This file is conditionally built if TCL support is enabled */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <tcl.h>

#include "tcl_lib.h"
#include "network.h"
#include "irc.h"
#include "util.h"
#include "trigger.h"
#include "trig_table.h"
#include "tcl_embed.h"

int tcl_rand(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[])
{
  int ret;
  int tcln;
  char *foo;

  if (objc != 2)
  {
    Tcl_WrongNumArgs(interp,objc,objv,"rand limit");
    return TCL_ERROR;
  }
 
  foo = tmalloc0(1024);

  srand(time(NULL));

  Tcl_GetIntFromObj(interp,objv[1],&tcln);

  /* BAD */
  ret = ((rand() % tcln) + 1)-1;

  /* C99 */
  snprintf(foo,1024,"%d",ret);

  Tcl_SetResult(interp,foo,NULL);

  return TCL_OK;
}

/* puthelp <message>
 * uses the queue
 */
int tcl_puthelp(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[])
{
  struct network *net = clientData;

  if (objc != 2)
  {
    Tcl_WrongNumArgs(interp,objc,objv,"puthelp message");
    return TCL_ERROR;
  }

  irc_printf(net->sock,"%s",Tcl_GetString(objv[1]));

  return TCL_OK;
}


/* putserv <message> 
 * bypasses the queue
 */
int tcl_putserv(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[])
{
  struct network *net = clientData;

  if (objc != 2)
  {
    Tcl_WrongNumArgs(interp,objc,objv,"putserv message");
    return TCL_ERROR;
  }

  irc_printf(net->sock,"%s",Tcl_GetString(objv[1]));

  return TCL_OK;
}

/* bind <type> <flags> <keyword/mask> [proc-name] */
int tcl_bind(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[]) 
{
  struct network *net = clientData;
  struct trigger *trig;

  if (objc != 5) 
  {
    Tcl_WrongNumArgs(interp,objc,objv,"bind type flags keyword proc");
    return TCL_ERROR;
  }
 
  if (!strcmp(Tcl_GetString(objv[1]),"pub"))
  {
    if (net->trigs->pub == NULL)
    {
      net->trigs->pub       = new_trigger(Tcl_GetString(objv[2]),TRIG_PUB,Tcl_GetString(objv[3]),Tcl_GetString(objv[4]),&tcl_handler);
      net->trigs->pub->prev = NULL;
      net->trigs->pub_head  = net->trigs->pub;
      net->trigs->pub_tail  = net->trigs->pub;
    }
    else
    {
      net->trigs->pub_tail->next = new_trigger(Tcl_GetString(objv[2]),TRIG_PUB,Tcl_GetString(objv[3]),Tcl_GetString(objv[4]),&tcl_handler);
      net->trigs->pub            = net->trigs->pub_tail->next;
      net->trigs->pub->prev      = net->trigs->pub_tail;
      net->trigs->pub_tail       = net->trigs->pub;      
    } 
     
    net->trigs->pub_tail->next = NULL;
  }
  else if (!strcmp(Tcl_GetString(objv[1]),"pubm"))
  {
    if (net->trigs->pubm == NULL)
    {
      net->trigs->pubm       = new_trigger(Tcl_GetString(objv[2]),TRIG_PUBM,Tcl_GetString(objv[3]),Tcl_GetString(objv[4]),&tcl_handler);
      net->trigs->pubm->prev = NULL;
      net->trigs->pubm_head  = net->trigs->pubm;
      net->trigs->pubm_tail  = net->trigs->pubm;
    }
    else
    {
      net->trigs->pubm_tail->next = new_trigger(Tcl_GetString(objv[2]),TRIG_PUBM,Tcl_GetString(objv[3]),Tcl_GetString(objv[4]),&tcl_handler);
      net->trigs->pubm            = net->trigs->pubm_tail->next;
      net->trigs->pubm->prev      = net->trigs->pubm_tail;
      net->trigs->pubm_tail       = net->trigs->pubm;
    }

    net->trigs->pubm_tail->next = NULL;
  }
  else if (!strcmp(Tcl_GetString(objv[1]),"msg"))
  {
    if (net->trigs->msg == NULL)
    {
      net->trigs->msg       = new_trigger(Tcl_GetString(objv[2]),TRIG_MSG,Tcl_GetString(objv[3]),Tcl_GetString(objv[4]),&tcl_handler);
      net->trigs->msg->prev = NULL;
      net->trigs->msg_head  = net->trigs->msg;
      net->trigs->msg_tail  = net->trigs->msg;
    }
    else
    {
      net->trigs->msg_tail->next = new_trigger(Tcl_GetString(objv[2]),TRIG_MSG,Tcl_GetString(objv[3]),Tcl_GetString(objv[4]),&tcl_handler);
      net->trigs->msg            = net->trigs->msg_tail->next;
      net->trigs->msg->prev      = net->trigs->msg_tail;
      net->trigs->msg_tail       = net->trigs->msg;
    }

    net->trigs->msg_tail->next = NULL;

  }  
  else if (!strcmp(Tcl_GetString(objv[1]),"msgm"))
  {
    if (net->trigs->msgm == NULL)
    {
      net->trigs->msgm       = new_trigger(Tcl_GetString(objv[2]),TRIG_MSGM,Tcl_GetString(objv[3]),Tcl_GetString(objv[4]),&tcl_handler);
      net->trigs->msgm->prev = NULL;
      net->trigs->msgm_head  = net->trigs->msgm;
      net->trigs->msgm_tail  = net->trigs->msgm;
    }
    else
    {
      net->trigs->msgm_tail->next = new_trigger(Tcl_GetString(objv[2]),TRIG_MSGM,Tcl_GetString(objv[3]),Tcl_GetString(objv[4]),&tcl_handler);
      net->trigs->msgm            = net->trigs->msgm_tail->next;
      net->trigs->msgm->prev      = net->trigs->msgm_tail;
      net->trigs->msgm_tail       = net->trigs->msgm;
    }

    net->trigs->msgm_tail->next = NULL;

  }
  else if (!strcmp(Tcl_GetString(objv[1]),"join"))
  {
    if (net->trigs->join == NULL)
    {
      net->trigs->join       = new_trigger(Tcl_GetString(objv[2]),TRIG_JOIN,Tcl_GetString(objv[3]),Tcl_GetString(objv[4]),&tcl_handler);
      net->trigs->join->prev = NULL;
      net->trigs->join_head  = net->trigs->join;
      net->trigs->join_tail  = net->trigs->join;
    }
    else
    {
      net->trigs->join_tail->next = new_trigger(Tcl_GetString(objv[2]),TRIG_JOIN,Tcl_GetString(objv[3]),Tcl_GetString(objv[4]),&tcl_handler);
      net->trigs->join            = net->trigs->join_tail->next;
      net->trigs->join->prev      = net->trigs->join_tail;
      net->trigs->join_tail       = net->trigs->join;
    }

    net->trigs->join_tail->next = NULL;

  }
  else if (!strcmp(Tcl_GetString(objv[1]),"part"))
  {
    if (net->trigs->part == NULL)
    {
      net->trigs->part       = new_trigger(Tcl_GetString(objv[2]),TRIG_PART,Tcl_GetString(objv[3]),Tcl_GetString(objv[4]),&tcl_handler);
      net->trigs->part->prev = NULL;
      net->trigs->part_head  = net->trigs->part;
      net->trigs->part_tail  = net->trigs->part;
    }
    else
    {
      net->trigs->part_tail->next = new_trigger(Tcl_GetString(objv[2]),TRIG_PART,Tcl_GetString(objv[3]),Tcl_GetString(objv[4]),&tcl_handler);
      net->trigs->part            = net->trigs->part_tail->next;
      net->trigs->part->prev      = net->trigs->part_tail;
      net->trigs->part_tail       = net->trigs->part;
    }

    net->trigs->part_tail->next = NULL;
  }
  else if (!strcmp(Tcl_GetString(objv[1]),"quit"))
  {
    if (net->trigs->quit == NULL)
    {
      net->trigs->quit       = new_trigger(Tcl_GetString(objv[2]),TRIG_QUIT,Tcl_GetString(objv[3]),Tcl_GetString(objv[4]),&tcl_handler);
      net->trigs->quit->prev = NULL;
      net->trigs->quit_head  = net->trigs->quit;
      net->trigs->quit_tail  = net->trigs->quit;
    }
    else
    {
      net->trigs->quit_tail->next = new_trigger(Tcl_GetString(objv[2]),TRIG_QUIT,Tcl_GetString(objv[3]),Tcl_GetString(objv[4]),&tcl_handler);
      net->trigs->quit            = net->trigs->quit_tail->next;
      net->trigs->quit->prev      = net->trigs->quit_tail;
      net->trigs->quit_tail       = net->trigs->quit;
    }

    net->trigs->quit_tail->next = NULL;
  }
  else
    return TCL_ERROR;

     
  return TCL_OK; 
}

