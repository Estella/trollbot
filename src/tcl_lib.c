/* This file is conditionally built if TCL support is enabled */
#include <time.h>

#include "main.h"
#include "tcl_lib.h"
#include "tcl_embed.h"

#include "network.h"
#include "irc_trigger.h"
#include "irc.h"
#include "troll_lib.h"
#include "egg_lib.h"
#include "user.h"

#ifdef HAVE_ICS
#include "ics_server.h"
#include "ics_proto.h"
#include "ics_game.h"
#include "ics_trigger.h"
#include "ics_lib.h"
#endif /* HAVE_ICS */


/* ICS specific eggdrop IRC commands */
#ifdef HAVE_ICS

int tcl_ics_get_score(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[])
{
	struct ics_server *ics = NULL;
	char *label;
	char *whom;
	int score;
	Tcl_Obj *t_score;

	if (objc != 3)
	{
		Tcl_WrongNumArgs(interp, objc, objv, "<label> <whom>");
		return TCL_ERROR;
	}

	label   = Tcl_GetString(objv[1]);
	whom    = Tcl_GetString(objv[2]);

	ics = g_cfg->ics_servers;

	while (ics != NULL)
	{
		if (!troll_matchwilds(ics->label, label))
		{
			score = ics_get_score(ics, whom);

			t_score = Tcl_NewIntObj(score);

			Tcl_SetObjResult(interp, t_score);

			break;
		}

		ics = ics->next;
	}
	
	return TCL_OK;
}

int tcl_putics(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[])
{
	struct ics_server *ics = NULL;
	char *label;
	char *message;

	if (objc != 3)
	{
		Tcl_WrongNumArgs(interp, objc, objv, "<label> <message>");
		return TCL_ERROR;
	}

	label   = Tcl_GetString(objv[1]);
	message = Tcl_GetString(objv[2]);

	ics = g_cfg->ics_servers;

	while (ics != NULL)
	{
		if (!troll_matchwilds(ics->label, label))
		{
			ics_printf(ics, message);
		}

		ics = ics->next;
	}
	
	return TCL_OK;
}

/* ICS commands only */
int tcl_ics_bind(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[])
{
	struct ics_server *ics = clientData;

	if (objc != 5) 
	{
		Tcl_WrongNumArgs(interp,objc,objv,"<type> <flags> <keyword> <proc>");
		return TCL_ERROR;
	}

	/* Pass it to egg_lib */
	if (!ics_bind(ics,Tcl_GetString(objv[1]),Tcl_GetString(objv[2]),Tcl_GetString(objv[3]),Tcl_GetString(objv[4]),ics_tcl_handler))
		return TCL_ERROR;

	return TCL_OK; 
}

int tcl_get_board(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[])
{
/*	char **board;

	if (objc != 2 || objc != 3)
	{
		Tcl_WrongNumArgs(interp, objc, objv, "<ics server> [game_id]");
		return TCL_ERROR;
	}


	ics_get_board(ics, -1);
*/
return TCL_OK;
}


int tcl_irc_interp(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[])
{
	struct network *net;
	char *net_name;
	Tcl_Interp *alt_interp;
	int ret;
	Tcl_Obj *nobjv[1];

	if (objc != 3)
	{
		Tcl_WrongNumArgs(interp, objc, objv, "<network name> <code to send>");
		return TCL_ERROR;
	}

	net_name = Tcl_GetString(objv[1]);

	net = g_cfg->networks;

	while (net != NULL)
	{
		if (!troll_matchwilds(net->label, net_name))
		{
			alt_interp = net->tclinterp;
		
			ret = Tcl_GlobalEvalObj(alt_interp, objv[2]);

			/* If we returned an error, send it to trollbot's warning channel */
			if (ret == TCL_ERROR)
			{
				troll_debug(LOG_WARN,"TCL Error: %s\n",Tcl_GetStringResult(alt_interp));
			}
		}

		net = net->next;
	}

	return TCL_OK;
}

int tcl_ics_interp(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[])
{
	struct ics_server *ics;
	char *ics_name;
	Tcl_Interp *alt_interp;
	int ret;
	Tcl_Obj *nobjv[1];

	if (objc != 3)
	{
		Tcl_WrongNumArgs(interp, objc, objv, "<ics name> <code to send>");
		return TCL_ERROR;
	}

	ics_name = Tcl_GetString(objv[1]);

	ics = g_cfg->ics_servers;

	while (ics != NULL)
	{
		if (!matchwilds(ics->label, ics_name))
		{
			alt_interp = ics->tclinterp;
		
			ret = Tcl_GlobalEvalObj(alt_interp, objv[2]);

			/* If we returned an error, send it to trollbot's warning channel */
			if (ret == TCL_ERROR)
			{
				troll_debug(LOG_WARN,"TCL Error: %s\n",Tcl_GetStringResult(alt_interp));
			}
		}

		ics = ics->next;
	}

	return TCL_OK;
}


#endif /* HAVE_ICS */

int tcl_chanbans(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[])
{
	struct network *net = clientData;
	Tcl_Obj *list;
	Tcl_Obj *scratch;
	
	char *channel = NULL;
	char **ret;
	int i;

	if (objc != 2)
	{
		Tcl_WrongNumArgs(interp, objc, objv, "<channel>");
		return TCL_ERROR;
	}

	channel = Tcl_GetString(objv[1]);

	ret = egg_chanbans(net, channel);

	if (ret == NULL)
	{
		return TCL_ERROR;
	}
	
	list = Tcl_NewListObj(0, NULL);
	Tcl_IncrRefCount(list);


	for (i=0;ret[i] != NULL && ret[i][0] != '\0';i++)
	{
		scratch = Tcl_NewStringObj(ret[i], strlen(ret[i]));
		Tcl_IncrRefCount(scratch);

		Tcl_ListObjAppendElement(interp, list, scratch);
		
		Tcl_DecrRefCount(scratch);
	}

	Tcl_SetObjResult(interp, list);
	
	tstrfreev(ret);
	
	return TCL_OK;
}
/*
  newchanban <channel> <ban> <creator> <comment> [lifetime] [options]
    Description: adds a ban to the ban list of a channel; creator is given
      credit for the ban in the ban list. lifetime is specified in
      minutes. If lifetime is not specified, ban-time (usually 60) is
      used. Setting the lifetime to 0 makes it a permanent ban.
*/
int tcl_newchanban(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[])
{
	struct network *net = clientData;
	char *channel = NULL;
	char *ban     = NULL;
	char *creator = NULL;
	char *comment = NULL;
	int lifetime  = -1;
	char *options = NULL;

	if (objc < 5 || objc > 7)
	{
		Tcl_WrongNumArgs(interp, objc, objv, "<channel> <ban> <creator> <comment> [lifetime] [options]");
		return TCL_ERROR;
	}

	channel = Tcl_GetString(objv[1]);
	ban     = Tcl_GetString(objv[2]);
	creator = Tcl_GetString(objv[3]);
	comment = Tcl_GetString(objv[4]);

	switch (objc)
	{
		case 6:
			Tcl_GetIntFromObj(interp,objv[5],&lifetime);
		case 7:
			options = Tcl_GetString(objv[6]);
		break;
	}

	egg_newchanban(net, channel, ban, creator, comment, lifetime, options);
	
	return TCL_OK;
}

int tcl_binds(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[])
{
	struct network *net = clientData;
	char **binds = NULL;
	char *mask   = NULL;
	char *ret    = NULL;

	if (objc > 2)
	{
		Tcl_WrongNumArgs(interp, objc, objv, "[type/mask]");
		return TCL_ERROR;
	}

	if (objc == 2)
		mask = Tcl_GetString(objv[1]);

	binds = egg_binds(net, mask);
	
	return TCL_OK;
}

/*   putlog <text>
 *   Description: sends text to the bot's logfile, marked as 'misc' (o)
 *   Returns: nothing
 *   Module: core
 */
int tcl_putlog(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[])
{
	struct network *net = clientData;
	char *text;

	if (objc != 2)
	{
		Tcl_WrongNumArgs(interp, objc, objv, "<text>");
		return TCL_ERROR;
	}

	text = Tcl_GetString(objv[1]);

	egg_putlog(net, text);
	
	return TCL_OK;
}

int tcl_putdcc(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[])
{
	struct network *net = clientData;
	char *text;
	int idx;

	if (objc != 3)
	{
		Tcl_WrongNumArgs(interp, objc, objv, "<idx> <text>");
		return TCL_ERROR;
	}

	Tcl_GetIntFromObj(interp,objv[1],&idx);
	text = Tcl_GetString(objv[2]);

	egg_putdcc(net, idx, text);
	
	return TCL_OK;
}

int tcl_isbotnick(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[])
{
	struct network *net = clientData;
	char *nick;
	int ret;

	if (objc != 2)
	{
		Tcl_WrongNumArgs(interp, objc, objv, "<nickname>");
		return TCL_ERROR;
	}

	nick = Tcl_GetString(objv[1]);

	ret = egg_isbotnick(net, nick);
	
	if (ret == 0)
		Tcl_SetResult(interp, "0", NULL);
	else
		Tcl_SetResult(interp, "1", NULL);

	return TCL_OK;
}

int tcl_adduser(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[])
{
	struct network *net = clientData;
	char *username;
	char *hostmask;
	int ret;

	if (objc < 2 || objc > 3)
	{
		Tcl_WrongNumArgs(interp, objc, objv, "<username> [hostmask]");
		return TCL_ERROR;
	}

	username = Tcl_GetString(objv[1]);
	hostmask = (objc == 3) ? Tcl_GetString(objv[2]) : NULL;

	ret = egg_adduser(net, username, hostmask);
	
	if (ret == 0)
		Tcl_SetResult(interp, "0", NULL);
	else
		Tcl_SetResult(interp, "1", NULL);

	return TCL_OK;
}


int tcl_isban(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[])
{
	struct network *net = clientData;
	char *ban;
	char *channel;
	int ret;

	if (objc < 2 || objc > 3)
	{
		Tcl_WrongNumArgs(interp, objc, objv, "<ban> [channel]");
		return TCL_ERROR;
	}

	ban     = Tcl_GetString(objv[1]);
	channel = (objc == 3) ? Tcl_GetString(objv[2]) : NULL;

	ret = egg_isban(net, ban, channel);
	
	if (ret == 0)
		Tcl_SetResult(interp, "0", NULL);
	else
		Tcl_SetResult(interp, "1", NULL);

	return TCL_OK;
}
	
int tcl_passwdok(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[])
{
	struct network *net = clientData;
	char *username;
	char *pass;
	int ret;

	if (objc != 3)
	{
		Tcl_WrongNumArgs(interp, objc, objv, "<handle> <password>");
		return TCL_ERROR;
	}

	username = Tcl_GetString(objv[1]);
	pass     = Tcl_GetString(objv[2]);

	ret = egg_passwdok(net, username, pass);

	if (ret == 0)
		Tcl_SetResult(interp,"0",NULL);
	else
		Tcl_SetResult(interp,"1",NULL);  

	return TCL_OK;
}
	
int tcl_unbind(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[])
{
	struct network *net = clientData;
	char *type;
	char *flags;
	char *mask;
	char *command;

	if (objc != 5)
	{
		Tcl_WrongNumArgs(interp, objc, objv, "<type> <flags> <keyword/mask> <proc-name>");
		return TCL_ERROR;
	}

	type    = Tcl_GetString(objv[1]);
	flags   = Tcl_GetString(objv[2]);
	mask    = Tcl_GetString(objv[3]);
	command = Tcl_GetString(objv[4]);

	egg_unbind(net, type, flags, mask, command);

	return TCL_OK;
}

int tcl_die(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[])
{
	struct network *net = clientData;
	char *reason;

	if (objc == 1)
	{
		egg_die(net, NULL);
		return TCL_OK;
	}

	if (objc > 1)
	{
		reason = Tcl_GetString(objv[1]);
		egg_die(net, reason);
	}

	/* Never reached */
	return TCL_OK;
}

int tcl_isvoice(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[])
{
	struct network *net = clientData;
	char *nickname;
	char *channel;

	if (objc == 2)
	{
		nickname = Tcl_GetString(objv[1]); 
		channel  = NULL;
	}
	else if (objc == 3)
	{
		nickname = Tcl_GetString(objv[2]);
		channel  = Tcl_GetString(objv[3]);
	}
	else
	{
		Tcl_WrongNumArgs(interp, objc, objv, "<nickname> [channel]");
		return TCL_ERROR;
	}

	/* If not found, return 0 */
	if (!egg_isvoice(net,nickname, channel))
		Tcl_SetResult(interp, "0", NULL);
	else
		Tcl_SetResult(interp, "1", NULL);

	return TCL_OK;
}

int tcl_isop(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[])
{
	struct network *net = clientData;
	char *nickname;
	char *channel;

	if (objc == 2)
	{
		nickname = Tcl_GetString(objv[1]); 
		channel  = NULL;
	}
	else if (objc == 3)
	{
		nickname = Tcl_GetString(objv[2]);
		channel  = Tcl_GetString(objv[3]);
	}
	else
	{
		Tcl_WrongNumArgs(interp, objc, objv, "<nickname> [channel]");
		return TCL_ERROR;
	}

	/* If not found, return 0 */
	if (!egg_isop(net,nickname, channel))
		Tcl_SetResult(interp, "0", NULL);
	else
		Tcl_SetResult(interp, "1", NULL);

	return TCL_OK;
}

int tcl_getchanmode(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[])
{
	struct network *net = clientData;
	char *ret;

	if (objc != 2)
	{
		Tcl_WrongNumArgs(interp, objc, objv, "<channel>");
		return TCL_ERROR;
	}

	ret = egg_getchanmode(net, Tcl_GetString(objv[1]));
	
	/* FIXME: Eggdrop reports "invalid channel: #channel" when it doesn't exist */
	if (ret == NULL)
		return TCL_ERROR;

	/* FIXME: Memory leak */
	Tcl_SetResult(interp, ret, NULL);

	return TCL_OK;
}

int tcl_encpass(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[])
{
	char *ret;
	
	if (objc != 2)
	{
		Tcl_WrongNumArgs(interp, objc, objv, "<password>");
		return TCL_ERROR;
	}

	ret = egg_encpass(Tcl_GetString(objv[1]));

	/* FIXME: Memory leak */
	Tcl_SetResult(interp, ret, NULL);
	
	return TCL_OK;
}

int tcl_topic(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[])
{
	struct network *net = clientData;
	char *ret;
	
	if (objc != 2)
	{
		Tcl_WrongNumArgs(interp, objc, objv, "<channel>");
		return TCL_ERROR;
	}

	ret = egg_topic(net, Tcl_GetString(objv[1]));

	/* FIXME: Memory leak */
	Tcl_SetResult(interp, ret, NULL);
	
	return TCL_OK;
}

int tcl_validuser(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[])
{
	struct network *net    = clientData;

	if (objc != 2)
	{
		Tcl_WrongNumArgs(interp, objc, objv,"<handle>");
		return TCL_ERROR;
	}

	/* If not found, return 0 */
	if (!egg_validuser(net,Tcl_GetString(objv[1])))
		Tcl_SetResult(interp, "0", NULL);
	else
		Tcl_SetResult(interp, "1", NULL);

	return TCL_OK;
}

int tcl_utimer(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[])
{
  struct network *net = clientData;
	int seconds   = -1;
	char *command = NULL;

	if (objc != 3)
	{
		Tcl_WrongNumArgs(interp, objc, objv,"<seconds> <command>");
		return TCL_ERROR;
	}

	seconds = atoi(Tcl_GetString(objv[1]));
	command = Tcl_GetString(objv[2]);

  troll_debug(LOG_DEBUG,"[TIMERS] Started Timer: Command %s",command);

	egg_utimer(net,seconds,command,t_timer_tcl_handler);

	return TCL_OK;
}

int tcl_savechannels(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[])
{
	struct network *net = clientData;

	egg_savechannels(net);

	return TCL_OK;
}

int tcl_finduser(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[])
{
	struct network *net  = clientData;
	struct user    *user = NULL;
	char *mask = NULL;
	char *ret  = NULL;

	if (objc != 2)
	{
		Tcl_WrongNumArgs(interp, objc, objv,"<mask>");
		return TCL_ERROR;
	}

	mask = Tcl_GetString(objv[1]);

	user = egg_finduser(net,mask);

	if (user == NULL)
	{
		Tcl_SetResult(interp, "*", NULL);
		return TCL_OK;
	}

	ret = tstrdup(user->username);

	/* FIXME: Memory Leak TCL_DYNAMIC for the free func causes crash */
	Tcl_SetResult(interp, ret, NULL);

	return TCL_OK;
}


int tcl_timer(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[])
{
	/* Initiate Timers */
	return TCL_OK;
}

int tcl_channels(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[])
{
	struct network *net = clientData;
	char *ret = NULL;

	ret = egg_channels(net);

	/* FIXME: Memory Leak TCL_DYNAMIC for the free func causes crash */
	Tcl_SetResult(interp, ret, NULL);

	return TCL_OK;
}

int tcl_save(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[])
{
	struct network *net = clientData;

	egg_save(net);

	return TCL_OK;
}

int tcl_botname(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[])
{
	char *ret = NULL;
	struct network *net = clientData;

	/* Ignore extra args rather than check it, check with eggdrop
	 * to see if this is how it acts.
	 */
	ret = egg_botname(net);

	/* FIXME: Memory Leak TCL_DYNAMIC for the free func causes crash */
	Tcl_SetResult(interp, ret, NULL);

	return TCL_OK;
}	


int tcl_onchan(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[])
{
	int ret;
	struct network *net = clientData;

	char *nickname = NULL;
	char *chan     = NULL;

	if (objc != 2 && objc != 3)
	{
		Tcl_WrongNumArgs(interp, objc, objv,"<nick> [channel]");
		return TCL_ERROR;
	}

	nickname = Tcl_GetString(objv[1]);
	chan     = Tcl_GetString(objv[2]);

	ret = egg_onchan(net,nickname,chan);

	if (ret == 0)
		Tcl_SetResult(interp,"0",NULL);
	else
		Tcl_SetResult(interp,"1",NULL);

	return TCL_OK;
}


int tcl_matchwild(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[])
{
	int ret;

	if (objc != 3)
	{
		Tcl_WrongNumArgs(interp,objc,objv,"<haystack> <needle>");
		return TCL_ERROR;
	}

	ret = troll_matchwilds(Tcl_GetString(objv[1]),Tcl_GetString(objv[2]));

	if (ret == 0)
		Tcl_SetResult(interp,"0",NULL);
	else
		Tcl_SetResult(interp,"1",NULL);  

	return TCL_OK;
}

int tcl_rand(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[])
{
	int ret;
	int tcln;
	char *foo;

	if (objc != 2)
	{
		Tcl_WrongNumArgs(interp,objc,objv,"<limit>");
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
		Tcl_WrongNumArgs(interp,objc,objv,"<message>");
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
		Tcl_WrongNumArgs(interp,objc,objv,"<message>");
		return TCL_ERROR;
	}

	irc_printf(net->sock,"%s",Tcl_GetString(objv[1]));

	return TCL_OK;
}

/* bind <type> <flags> <keyword/mask> [proc-name] */
int tcl_bind(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[]) 
{
	struct network *net = clientData;

	if (objc != 5) 
	{
		Tcl_WrongNumArgs(interp,objc,objv,"<type> <flags> <keyword> <proc>");
		return TCL_ERROR;
	}

	/* Pass it to egg_lib */
	if (!egg_bind(net,Tcl_GetString(objv[1]),Tcl_GetString(objv[2]),Tcl_GetString(objv[3]),Tcl_GetString(objv[4]),tcl_handler))
		return TCL_ERROR;

	return TCL_OK; 
}

int tcl_matchattr(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[])
{
	Tcl_Obj *ret;
	struct network *net;

	net = clientData;

	if (objc != 3 && objc != 4)
	{
		Tcl_WrongNumArgs(interp,objc,objv,"<handle> <flags> [channel]");
		return TCL_ERROR;
	}

	/* No channel flag support FIXME */
	ret = Tcl_NewIntObj(egg_matchattr(net,Tcl_GetString(objv[1]),Tcl_GetString(objv[2]),NULL));

	Tcl_SetObjResult(interp,ret);

	return TCL_OK;
}


int tcl_countusers(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[])
{
	Tcl_Obj *ret;
	struct network *net;

	net = clientData;

	ret = Tcl_NewIntObj(egg_countusers(net));

	Tcl_SetObjResult(interp,ret);  

	return TCL_OK;
}

int tcl_ispermban(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[])
{
	Tcl_Obj *ret;
	struct network *net;

	net = clientData;

	if (objc != 2 && objc != 3)
	{
		Tcl_WrongNumArgs(interp,objc,objv,"<ban> [channel]");
		return TCL_ERROR;
	}

	if (objc == 2)
		ret = Tcl_NewIntObj(egg_ispermban(net, Tcl_GetString(objv[1]), NULL));
	else
		ret = Tcl_NewIntObj(egg_ispermban(net, Tcl_GetString(objv[1]), Tcl_GetString(objv[2])));

	Tcl_SetObjResult(interp,ret);  

	return TCL_OK;
}


