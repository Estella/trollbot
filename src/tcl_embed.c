#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <tcl.h>

#include "util.h"
#include "network.h"
#include "irc.h"
#include "tcl_embed.h"
#include "tcl_lib.h"
#include "egglib.h"

void net_init_tcl(struct network *net)
{  
  net->tclinterp = Tcl_CreateInterp();

  Tcl_CreateObjCommand(net->tclinterp,
                       "matchwild",
                       tcl_matchwild,
                       net,
                       NULL);

  Tcl_CreateObjCommand(net->tclinterp,
                       "putserv",
                       tcl_putserv,
                       net,
                       NULL);

  Tcl_CreateObjCommand(net->tclinterp,
                       "puthelp",
                       tcl_puthelp,
                       net,
                       NULL);

  Tcl_CreateObjCommand(net->tclinterp,
                       "rand",
                       tcl_rand,
                       net,
                       NULL);


  Tcl_CreateObjCommand(net->tclinterp,
                       "bind",
                       tcl_bind,
                       net,
                       NULL);

}

void tcl_handler(struct network *net, struct trigger *trig, struct irc_data *data)
{
  int ret;

  printf("Triggered\n");
  switch (trig->type)
  {
    case TRIG_PUB:      
      /* nick uhost hand chan arg */
      ret = Tcl_VarEval(net->tclinterp,
                        trig->command,
                        " {",
                        data->prefix->nick,
                        "} {",
                        data->prefix->host,
                        "} {} {", /* hand */
                        data->c_params[0], /* chan */
                        "} {", 
                        egg_makearg(data->rest_str,trig->mask),
                        "}",
                        NULL);

      if (ret == TCL_ERROR)
      {
        printf("TCL Error: %s\n",net->tclinterp->result);
      }

      break;
    case TRIG_PUBM:
      /* nick uhost hand chan arg */
      ret = Tcl_VarEval(net->tclinterp,
                        trig->command,
                        " {",
                        data->prefix->nick,
                        "} {",
                        data->prefix->host,
                        "} {} {", /* hand */
                        data->c_params[0], /* chan */
                        "} {",
                        data->rest_str,
                        "}",
                        NULL);

      if (ret == TCL_ERROR)
      {
        printf("TCL Error: %s\n",net->tclinterp->result);
      }

      break;
    case TRIG_MSG:
      ret = Tcl_VarEval(net->tclinterp,
                        trig->command,
                        " {",
                        data->prefix->nick,
                        "} {",
                        data->prefix->host,
                        "} {} {", /* hand */
                        ((&data->rest_str[strlen(trig->mask)] == NULL) || &data->rest_str[strlen(trig->mask)+1] == NULL) ? "" : &data->rest_str[strlen(trig->mask)+1],
                        "}",
                        NULL);

      if (ret == TCL_ERROR)
      {
        printf("TCL Error: %s\n",net->tclinterp->result);
      }

      break;
    case TRIG_MSGM:
      break;
    case TRIG_JOIN:
      break;
    case TRIG_PART:
      break;
    case TRIG_QUIT:
      break;
  }  
}
