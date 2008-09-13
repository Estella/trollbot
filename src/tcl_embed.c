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
#include "troll_lib.h"

#ifdef HAVE_ICS
#include "ics_server.h"
#include "ics_proto.h"
#include "ics_game.h"
#include "ics_trigger.h"
#endif /* HAVE_ICS */


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
	Tcl_Init(net->tclinterp);
	net_tcl_init_commands(net);
}

void net_tcl_init_commands(struct network *net)
{
#ifdef HAVE_ICS
	Tcl_CreateObjCommand(net->tclinterp, "putics", tcl_putics, net, NULL);
#endif /* HAVE_ICS */

	Tcl_CreateObjCommand(net->tclinterp, "binds", tcl_binds, net, NULL);
	Tcl_CreateObjCommand(net->tclinterp, "newchanban", tcl_newchanban, net, NULL);
	Tcl_CreateObjCommand(net->tclinterp, "putlog", tcl_putlog, net, NULL);
	Tcl_CreateObjCommand(net->tclinterp, "putdcc", tcl_putdcc, net, NULL);
	Tcl_CreateObjCommand(net->tclinterp, "isbotnick", tcl_isbotnick, net, NULL);
	Tcl_CreateObjCommand(net->tclinterp, "adduser", tcl_adduser, net, NULL);
	Tcl_CreateObjCommand(net->tclinterp, "isban", tcl_isban, net, NULL);
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

/* This handles all triggers which have a handler of tcl, or was set that way through
 * a bind in a TCL script. 
 *
 * Rewritten to use the proper way, instead of doing that Tcl_ValEval() garbage.
 */
void tcl_handler(struct network *net, struct trigger *trig, struct irc_data *data, struct dcc_session *dcc, const char *dccbuf)
{
	int ret;
	char *my_arg;
	Tcl_Obj *command;
	Tcl_Obj *nick;
	Tcl_Obj *uhost;
	Tcl_Obj *hand;
	Tcl_Obj *chan;
	Tcl_Obj *arg;
	Tcl_Obj *msg;
	Tcl_Obj *from;
	Tcl_Obj *keyword;
	Tcl_Obj *text;
	Tcl_Obj **objv;

	switch (trig->type)
	{
		case TRIG_PUB: 
			/* The proper way of doing things, according to #tcl on freenode (they'd know) */
			command = Tcl_NewStringObj(trig->command,      strlen(trig->command));
			nick    = Tcl_NewStringObj(data->prefix->nick, strlen(data->prefix->nick));
			uhost   = Tcl_NewStringObj(data->prefix->host, strlen(data->prefix->host));
			hand    = Tcl_NewStringObj(data->prefix->nick, strlen(data->prefix->nick));
			chan    = Tcl_NewStringObj(data->c_params[0],  strlen(data->c_params[0]));

			/* We do this because I'm retarded and have no way of figuring out what should happen after the mask */
			my_arg = tstrdup(troll_makearg(data->rest_str,trig->mask));
			arg     = Tcl_NewStringObj(my_arg, strlen(my_arg));

			/* We need to increase the reference count, because if TCL suddenly gets some
			 * time for GC, it will notice a zero reference count
			 */
			Tcl_IncrRefCount(command);
			Tcl_IncrRefCount(nick);
			Tcl_IncrRefCount(uhost);
			Tcl_IncrRefCount(hand);
			Tcl_IncrRefCount(chan);
			Tcl_IncrRefCount(arg);

			/* I don't need a NULL last array element */
			objv = tmalloc(sizeof(Tcl_Obj *) * 6);

			objv[0] = command;
			objv[1] = nick;
			objv[2] = uhost;
			objv[3] = hand;
			objv[4] = chan;
			objv[5] = arg;
	
			/* Call <command> <nick> <uhost> <hand> <chan> <arg> */
			ret = Tcl_EvalObjv(net->tclinterp, 6, objv, TCL_EVAL_GLOBAL);

			/* Decrement the reference count so the GC will catch it */
			Tcl_DecrRefCount(command);
			Tcl_DecrRefCount(nick);
			Tcl_DecrRefCount(uhost);
			Tcl_DecrRefCount(hand);
			Tcl_DecrRefCount(chan);
			Tcl_DecrRefCount(arg);

			/* If we returned an error, send it to trollbot's warning channel */
			if (ret == TCL_ERROR)
			{
				troll_debug(LOG_WARN,"TCL Error: %s\n",Tcl_GetStringResult(net->tclinterp));
			}

			free(my_arg);
			free(objv);

			break;
		case TRIG_PUBM:
			/* The proper way of doing things, according to #tcl on freenode (they'd know) */
			command = Tcl_NewStringObj(trig->command,      strlen(trig->command));
			nick    = Tcl_NewStringObj(data->prefix->nick, strlen(data->prefix->nick));
			uhost   = Tcl_NewStringObj(data->prefix->host, strlen(data->prefix->host));
			hand    = Tcl_NewStringObj(data->prefix->nick, strlen(data->prefix->nick));
			chan    = Tcl_NewStringObj(data->c_params[0],  strlen(data->c_params[0]));
			text    = Tcl_NewStringObj(data->rest_str,     strlen(data->rest_str));

			/* We need to increase the reference count, because if TCL suddenly gets some
			 * time for GC, it will notice a zero reference count
			 */
			Tcl_IncrRefCount(command);
			Tcl_IncrRefCount(nick);
			Tcl_IncrRefCount(uhost);
			Tcl_IncrRefCount(hand);
			Tcl_IncrRefCount(chan);
			Tcl_IncrRefCount(text);

			/* I don't need a NULL last array element */
			objv = tmalloc(sizeof(Tcl_Obj *) * 6);

			objv[0] = command;
			objv[1] = nick;
			objv[2] = uhost;
			objv[3] = hand;
			objv[4] = chan;
			objv[5] = text;
	
			/* Call <command> <nick> <uhost> <hand> <chan> <arg> */
			ret = Tcl_EvalObjv(net->tclinterp, 6, objv, TCL_EVAL_GLOBAL);

			/* Decrement the reference count so the GC will catch it */
			Tcl_DecrRefCount(command);
			Tcl_DecrRefCount(nick);
			Tcl_DecrRefCount(uhost);
			Tcl_DecrRefCount(hand);
			Tcl_DecrRefCount(chan);
			Tcl_DecrRefCount(text);

			/* If we returned an error, send it to trollbot's warning channel */
			if (ret == TCL_ERROR)
			{
				troll_debug(LOG_WARN,"TCL Error: %s\n",Tcl_GetStringResult(net->tclinterp));
			}

			free(objv);

			break;
		case TRIG_MSG:
			/* The proper way of doing things, according to #tcl on freenode (they'd know) */
			command = Tcl_NewStringObj(trig->command,      strlen(trig->command));
			nick    = Tcl_NewStringObj(data->prefix->nick, strlen(data->prefix->nick));
			uhost   = Tcl_NewStringObj(data->prefix->host, strlen(data->prefix->host));
			hand    = Tcl_NewStringObj(data->prefix->nick, strlen(data->prefix->nick));

			/* This is stupid, I don't even remember why the hell I did this */
			my_arg  = ((&data->rest_str[strlen(trig->mask)] == NULL) || &data->rest_str[strlen(trig->mask)+1] == NULL) ? "" : &data->rest_str[strlen(trig->mask)+1];
			text    = Tcl_NewStringObj(my_arg,     strlen(my_arg));

			/* We need to increase the reference count, because if TCL suddenly gets some
			 * time for GC, it will notice a zero reference count
			 */
			Tcl_IncrRefCount(command);
			Tcl_IncrRefCount(nick);
			Tcl_IncrRefCount(uhost);
			Tcl_IncrRefCount(hand);
			Tcl_IncrRefCount(text);

			/* I don't need a NULL last array element */
			objv = tmalloc(sizeof(Tcl_Obj *) * 5);

			objv[0] = command;
			objv[1] = nick;
			objv[2] = uhost;
			objv[3] = hand;
			objv[4] = text;
	
			/* Call <command> <nick> <uhost> <hand> <chan> <arg> */
			ret = Tcl_EvalObjv(net->tclinterp, 5, objv, TCL_EVAL_GLOBAL);

			/* Decrement the reference count so the GC will catch it */
			Tcl_DecrRefCount(command);
			Tcl_DecrRefCount(nick);
			Tcl_DecrRefCount(uhost);
			Tcl_DecrRefCount(hand);
			Tcl_DecrRefCount(text);

			/* If we returned an error, send it to trollbot's warning channel */
			if (ret == TCL_ERROR)
			{
				troll_debug(LOG_WARN,"TCL Error: %s\n",Tcl_GetStringResult(net->tclinterp));
			}

			free(objv);
			break;
		case TRIG_MSGM:
			/* The proper way of doing things, according to #tcl on freenode (they'd know) */
			command = Tcl_NewStringObj(trig->command,      strlen(trig->command));
			nick    = Tcl_NewStringObj(data->prefix->nick, strlen(data->prefix->nick));
			uhost   = Tcl_NewStringObj(data->prefix->host, strlen(data->prefix->host));
			hand    = Tcl_NewStringObj(data->prefix->nick, strlen(data->prefix->nick));

			/* This is stupid, I don't even remember why the hell I did this */
			text    = Tcl_NewStringObj(data->rest_str,     strlen(data->rest_str));

			/* We need to increase the reference count, because if TCL suddenly gets some
			 * time for GC, it will notice a zero reference count
			 */
			Tcl_IncrRefCount(command);
			Tcl_IncrRefCount(nick);
			Tcl_IncrRefCount(uhost);
			Tcl_IncrRefCount(hand);
			Tcl_IncrRefCount(text);

			/* I don't need a NULL last array element */
			objv = tmalloc(sizeof(Tcl_Obj *) * 5);

			objv[0] = command;
			objv[1] = nick;
			objv[2] = uhost;
			objv[3] = hand;
			objv[4] = text;
	
			/* Call <command> <nick> <uhost> <hand> <chan> <arg> */
			ret = Tcl_EvalObjv(net->tclinterp, 5, objv, TCL_EVAL_GLOBAL);

			/* Decrement the reference count so the GC will catch it */
			Tcl_DecrRefCount(command);
			Tcl_DecrRefCount(nick);
			Tcl_DecrRefCount(uhost);
			Tcl_DecrRefCount(hand);
			Tcl_DecrRefCount(text);

			/* If we returned an error, send it to trollbot's warning channel */
			if (ret == TCL_ERROR)
			{
				troll_debug(LOG_WARN,"TCL Error: %s\n",Tcl_GetStringResult(net->tclinterp));
			}

			free(objv);
			break;
		case TRIG_TOPC:
			/* The proper way of doing things, according to #tcl on freenode (they'd know) */
			command = Tcl_NewStringObj(trig->command,      strlen(trig->command));
			nick    = Tcl_NewStringObj(data->prefix->nick, strlen(data->prefix->nick));
			uhost   = Tcl_NewStringObj(data->prefix->host, strlen(data->prefix->host));
			hand    = Tcl_NewStringObj(data->prefix->nick, strlen(data->prefix->nick));
			chan    = Tcl_NewStringObj(data->c_params[0],  strlen(data->c_params[0]));
			text    = Tcl_NewStringObj(data->rest_str,     strlen(data->rest_str));

			/* We need to increase the reference count, because if TCL suddenly gets some
			 * time for GC, it will notice a zero reference count
			 */
			Tcl_IncrRefCount(command);
			Tcl_IncrRefCount(nick);
			Tcl_IncrRefCount(uhost);
			Tcl_IncrRefCount(hand);
			Tcl_IncrRefCount(chan);
			Tcl_IncrRefCount(text);

			/* I don't need a NULL last array element */
			objv = tmalloc(sizeof(Tcl_Obj *) * 6);

			objv[0] = command;
			objv[1] = nick;
			objv[2] = uhost;
			objv[3] = hand;
			objv[4] = chan;
			objv[5] = text;
	
			/* Call <command> <nick> <uhost> <hand> <chan> <arg> */
			ret = Tcl_EvalObjv(net->tclinterp, 6, objv, TCL_EVAL_GLOBAL);

			/* Decrement the reference count so the GC will catch it */
			Tcl_DecrRefCount(command);
			Tcl_DecrRefCount(nick);
			Tcl_DecrRefCount(uhost);
			Tcl_DecrRefCount(hand);
			Tcl_DecrRefCount(chan);
			Tcl_DecrRefCount(text);

			/* If we returned an error, send it to trollbot's warning channel */
			if (ret == TCL_ERROR)
			{
				troll_debug(LOG_WARN,"TCL Error: %s\n",Tcl_GetStringResult(net->tclinterp));
			}

			free(objv);
			break;
		case TRIG_RAW:
			/* The proper way of doing things, according to #tcl on freenode (they'd know) */
			command = Tcl_NewStringObj(trig->command,      strlen(trig->command));
			from    = Tcl_NewStringObj(data->c_params[0],  strlen(data->c_params[0]));
			keyword = Tcl_NewStringObj(trig->command,      strlen(trig->command));
			text    = Tcl_NewStringObj(data->rest_str,     strlen(data->rest_str));

			/* We need to increase the reference count, because if TCL suddenly gets some
			 * time for GC, it will notice a zero reference count
			 */
			Tcl_IncrRefCount(command);
			Tcl_IncrRefCount(from);
			Tcl_IncrRefCount(keyword);
			Tcl_IncrRefCount(text);

			/* I don't need a NULL last array element */
			objv = tmalloc(sizeof(Tcl_Obj *) * 4);

			objv[0] = command;
			objv[1] = from;
			objv[2] = keyword;
			objv[3] = text;
	
			/* Call <command> <nick> <uhost> <hand> <chan> <arg> */
			ret = Tcl_EvalObjv(net->tclinterp, 4, objv, TCL_EVAL_GLOBAL);

			/* Decrement the reference count so the GC will catch it */
			Tcl_DecrRefCount(command);
			Tcl_DecrRefCount(from);
			Tcl_DecrRefCount(keyword);
			Tcl_DecrRefCount(text);

			/* If we returned an error, send it to trollbot's warning channel */
			if (ret == TCL_ERROR)
			{
				troll_debug(LOG_WARN,"TCL Error: %s\n",Tcl_GetStringResult(net->tclinterp));
			}

			free(objv);
			break;
		/* :dfisfisd!ben@192.168.2.5 JOIN :#test */
		case TRIG_JOIN:
			/* The proper way of doing things, according to #tcl on freenode (they'd know) */
			command = Tcl_NewStringObj(trig->command,      strlen(trig->command));
			nick    = Tcl_NewStringObj(data->prefix->nick, strlen(data->prefix->nick));
			uhost   = Tcl_NewStringObj(data->prefix->host, strlen(data->prefix->host));
			hand    = Tcl_NewStringObj(data->prefix->nick, strlen(data->prefix->nick));
			chan    = Tcl_NewStringObj(data->rest_str,     strlen(data->rest_str));

			/* We need to increase the reference count, because if TCL suddenly gets some
			 * time for GC, it will notice a zero reference count
			 */
			Tcl_IncrRefCount(command);
			Tcl_IncrRefCount(nick);
			Tcl_IncrRefCount(uhost);
			Tcl_IncrRefCount(hand);
			Tcl_IncrRefCount(chan);


			/* I don't need a NULL last array element */
			objv = tmalloc(sizeof(Tcl_Obj *) * 5);

			objv[0] = command;
			objv[1] = nick;
			objv[2] = uhost;
			objv[3] = hand;
			objv[4] = chan;

			/* Call <command> <nick> <uhost> <hand> <chan> <arg> */
			ret = Tcl_EvalObjv(net->tclinterp, 5, objv, TCL_EVAL_GLOBAL);

			/* Decrement the reference count so the GC will catch it */
			Tcl_DecrRefCount(command);
			Tcl_DecrRefCount(nick);
			Tcl_DecrRefCount(uhost);
			Tcl_DecrRefCount(hand);
			Tcl_DecrRefCount(chan);

			/* If we returned an error, send it to trollbot's warning channel */
			if (ret == TCL_ERROR)
			{
				troll_debug(LOG_WARN,"TCL Error: %s\n",Tcl_GetStringResult(net->tclinterp));
			}

			free(objv);
	
			break;
		/* :dfisfisd!ben@192.168.2.5 PART #boo :eat my shit */
		case TRIG_PART:
			/* The proper way of doing things, according to #tcl on freenode (they'd know) */
			command = Tcl_NewStringObj(trig->command,      strlen(trig->command));
			nick    = Tcl_NewStringObj(data->prefix->nick, strlen(data->prefix->nick));
			uhost   = Tcl_NewStringObj(data->prefix->host, strlen(data->prefix->host));
			hand    = Tcl_NewStringObj(data->prefix->nick, strlen(data->prefix->nick));
			chan    = Tcl_NewStringObj(data->c_params[0],  strlen(data->c_params[0]));
			msg     = Tcl_NewStringObj(data->rest_str,     strlen(data->rest_str));


			/* We need to increase the reference count, because if TCL suddenly gets some
			 * time for GC, it will notice a zero reference count
			 */
			Tcl_IncrRefCount(command);
			Tcl_IncrRefCount(nick);
			Tcl_IncrRefCount(uhost);
			Tcl_IncrRefCount(hand);
			Tcl_IncrRefCount(chan);
			Tcl_IncrRefCount(msg);


			/* I don't need a NULL last array element */
			objv = tmalloc(sizeof(Tcl_Obj *) * 6);

			objv[0] = command;
			objv[1] = nick;
			objv[2] = uhost;
			objv[3] = hand;
			objv[4] = chan;
			objv[5] = msg;

			/* Call <command> <nick> <uhost> <hand> <chan> <arg> */
			ret = Tcl_EvalObjv(net->tclinterp, 6, objv, TCL_EVAL_GLOBAL);

			/* Decrement the reference count so the GC will catch it */
			Tcl_DecrRefCount(command);
			Tcl_DecrRefCount(nick);
			Tcl_DecrRefCount(uhost);
			Tcl_DecrRefCount(hand);
			Tcl_DecrRefCount(chan);
			Tcl_DecrRefCount(msg);


			/* If we returned an error, send it to trollbot's warning channel */
			if (ret == TCL_ERROR)
			{
				troll_debug(LOG_WARN,"TCL Error: %s\n",Tcl_GetStringResult(net->tclinterp));
			}

			free(objv);
	
			break;
	}  
}
