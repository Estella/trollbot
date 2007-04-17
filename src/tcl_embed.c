#include "main.h"

#include "tcl_embed.h"
#include "tcl_lib.h"

#include "trigger.h"
#include "irc.h"
#include "network.h"
#include "server.h"
#include "channel.h"
#include "user.h"

#include "egg_lib.h"

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

  Tcl_CreateObjCommand(net->tclinterp,
                       "matchattr",
                       tcl_matchattr,
                       net,
                       NULL);

  Tcl_CreateObjCommand(net->tclinterp,
                       "countusers",
                       tcl_countusers,
                       net,
                       NULL);

}

/* needs fixed FIXME */
void tcl_handler(struct network *net, struct trigger *trig, struct irc_data *data, struct dcc_session *dcc, const char *dccbuf)
{
  int ret;

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
        troll_debug(LOG_WARN,"TCL Error: %s\n",net->tclinterp->result);
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
        troll_debug(LOG_WARN,"TCL Error: %s\n",net->tclinterp->result);
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
        troll_debug(LOG_WARN,"TCL Error: %s\n",net->tclinterp->result);
      }

      break;
    case TRIG_MSGM:
      break;
    case TRIG_JOIN:
      break;
    case TRIG_PART:
      break;
    case TRIG_SIGN:
      break;
  }  
}
