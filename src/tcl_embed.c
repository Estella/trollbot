#include "main.h"

#include "tcl_embed.h"
#include "tcl_lib.h"

#include "trigger.h"
#include "irc.h"
#include "network.h"
#include "server.h"
#include "channel.h"
#include "user.h"
#include "dcc.h"
#include "t_timer.h"

#include "egg_lib.h"

void dcc_tcl(struct network *net, struct trigger *trig, struct irc_data *data, struct dcc_session *dcc, const char *dccbuf)
{
	int ret;

	ret = Tcl_VarEval(dcc->net->tclinterp, troll_makearg(dccbuf,trig->mask), NULL);

	if (ret == TCL_ERROR)
	{
		irc_printf(dcc->sock, "TCL Error: %s", dcc->net->tclinterp->result);
	}

	return;
}

void tcl_load_scripts_from_config(struct config *cfg)
{
	int i;
	struct network *net = cfg->networks;

	while (net != NULL)
	{
		if (net->tcl_scripts != NULL)
		{
			if (net->tclinterp == NULL)
				net_init_tcl(net);
			else
				net_tcl_init_commands(net);

			for (i=0;net->tcl_scripts[i] != NULL;i++)
			{
				if (Tcl_EvalFile(net->tclinterp, net->tcl_scripts[i]) == TCL_ERROR)
				{
					troll_debug(LOG_WARN,"TCL Error: %s\n",Tcl_GetStringResult(net->tclinterp));
				}
			}
		}

		net = net->next;
	}
}

void net_init_tcl(struct network *net)
{  
	net->tclinterp = Tcl_CreateInterp();
	net_tcl_init_commands(net);
}

void net_tcl_init_commands(struct network *net)
{
	Tcl_CreateObjCommand(net->tclinterp, "passwdok", tcl_passwdok, net, NULL);
	Tcl_CreateObjCommand(net->tclinterp, "unbind", tcl_unbind, net, NULL);
	Tcl_CreateObjCommand(net->tclinterp, "die", tcl_die, net, NULL);
	Tcl_CreateObjCommand(net->tclinterp, "utimer",tcl_utimer, net, NULL);
	Tcl_CreateObjCommand(net->tclinterp, "isop", tcl_isop, net, NULL);
	Tcl_CreateObjCommand(net->tclinterp, "isvoice", tcl_isvoice, net, NULL);
	Tcl_CreateObjCommand(net->tclinterp, "getchanmode", tcl_getchanmode, net, NULL);
	Tcl_CreateObjCommand(net->tclinterp, "encpass", tcl_encpass, net, NULL);
	Tcl_CreateObjCommand(net->tclinterp, "topic", tcl_topic, net, NULL);
	Tcl_CreateObjCommand(net->tclinterp, "validuser", tcl_validuser, net, NULL);
	Tcl_CreateObjCommand(net->tclinterp, "savechannels", tcl_savechannels, net, NULL);
	Tcl_CreateObjCommand(net->tclinterp, "finduser", tcl_finduser, net, NULL);
	Tcl_CreateObjCommand(net->tclinterp, "channels", tcl_channels, net, NULL);
	Tcl_CreateObjCommand(net->tclinterp, "save", tcl_save, net, NULL);
	Tcl_CreateObjCommand(net->tclinterp, "botname", tcl_botname, net, NULL);
	Tcl_CreateObjCommand(net->tclinterp, "onchan", tcl_onchan, net, NULL);
	Tcl_CreateObjCommand(net->tclinterp, "matchwild", tcl_matchwild, net, NULL);
	Tcl_CreateObjCommand(net->tclinterp, "putserv", tcl_putserv, net, NULL);
	Tcl_CreateObjCommand(net->tclinterp, "puthelp", tcl_puthelp, net, NULL);
	Tcl_CreateObjCommand(net->tclinterp, "rand", tcl_rand, net, NULL);
	Tcl_CreateObjCommand(net->tclinterp, "bind", tcl_bind, net, NULL);
	Tcl_CreateObjCommand(net->tclinterp, "matchattr", tcl_matchattr, net, NULL); 
	Tcl_CreateObjCommand(net->tclinterp, "countusers", tcl_countusers, net, NULL);
}

void t_timer_tcl_handler(struct network *net, struct t_timer *timer)
{
	int ret;

	ret = Tcl_VarEval(net->tclinterp,
										timer->command,
										NULL);

	if (ret == TCL_ERROR)
	{
		troll_debug(LOG_WARN,"TCL Error: %s\n",net->tclinterp->result);
	}
	
	return;
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
					troll_makearg(data->rest_str,trig->mask),
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
		case TRIG_TOPC:
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
		case TRIG_MSGM:
			break;
		case TRIG_JOIN:
			break;
		case TRIG_PART:
			break;
		case TRIG_SIGN:
			break;
		case TRIG_DCC:
			break;
	}  
}
