/* This file is conditionally built if TCL support is enabled */
#include <time.h>

#include "main.h"
#include "tcl_lib.h"
#include "tcl_embed.h"

#include "network.h"
#include "trigger.h"
#include "irc.h"
#include "egg_lib.h"
#include "user.h"

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
		Tcl_WrongNumArgs(interp, objc, objv, "isop <nickname> [channel]");
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
		Tcl_WrongNumArgs(interp, objc, objv, "isop <nickname> [channel]");
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
		Tcl_WrongNumArgs(interp, objc, objv, "getchanmode <channel>");
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
		Tcl_WrongNumArgs(interp, objc, objv, "encpass <password>");
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
		Tcl_WrongNumArgs(interp, objc, objv, "topic <channel>");
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
		Tcl_WrongNumArgs(interp, objc, objv,"validuser <handle>");
		return TCL_ERROR;
	}

	/* If not found, return 0 */
	if (!egg_validuser(net,Tcl_GetString(objv[1])))
		Tcl_SetResult(interp, "0", NULL);
	else
		Tcl_SetResult(interp, "1", NULL);

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
		Tcl_WrongNumArgs(interp, objc, objv,"finduser <mask>");
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

int tcl_utimer(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[])
{
	/* Initiate Timers */
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
		Tcl_WrongNumArgs(interp, objc, objv,"onchan <nick> [channel]");
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
		Tcl_WrongNumArgs(interp,objc,objv,"matchwild haystack needle");
		return TCL_ERROR;
	}

	ret = egg_matchwilds(Tcl_GetString(objv[1]),Tcl_GetString(objv[2]));

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

	if (objc != 5) 
	{
		Tcl_WrongNumArgs(interp,objc,objv,"bind type flags keyword proc");
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
		Tcl_WrongNumArgs(interp,objc,objv,"matchattr <handle> <flags> [channel]");
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

