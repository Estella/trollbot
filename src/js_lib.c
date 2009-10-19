#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <jsapi.h>

#include "js_lib.h"
#include "js_embed.h"
#include "irc_trigger.h"	
#include "util.h"
#include "user.h"
#include "dcc.h"
#include "irc.h"
#include "irc_network.h"
#include "egg_lib.h"

JSBool js_hand2idx(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	struct network *net     = JS_GetContextPrivate(cx);
	char *handle            = NULL;
	int ret                 = 0;

	/* 1 arg, exactly */
	if (argc != 1)
	{
		return JS_FALSE;
	}

	handle = JS_GetStringBytes(JS_ValueToString(cx, argv[0]));
		
	ret = egg_hand2idx(net, handle);

	*rval = INT_TO_JSVAL(ret);

	return JS_TRUE;
}

#ifdef CLOWNS
/* Need to know how to return arrays in JS */
JSBool js_chanbans(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	struct network *net     = JS_GetContextPrivate(cx);
	char           *channel = NULL;
	struct channel *chan;
}

#endif /* CLOWNS */

#ifdef CLOWNS
/* newchanban <channel> <ban> <creator> <comment> [lifetime] [options] */
JSBool js_newchanban(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	struct network *net     = JS_GetContextPrivate(cx);
	char           *channel = NULL;
	char           *ban     = NULL;
	char           *creator = NULL;
	char           *comment = NULL;
	int            lifetime = NULL;
	char           *option  = NULL;

	/* Not that this will be used */	
	*rval = JSVAL_VOID;

	/* 4-6 args */
	if (argc <= 5 || argc > 7)
	{
		return JS_FALSE;
	}

	channel = JS_GetStringBytes(JS_ValueToString(cx, argv[0]));
	ban     = JS_GetStringBytes(JS_ValueToString(cx, argv[0]));
	creator = JS_GetStringBytes(JS_ValueToString(cx, argv[0]));
	comment = JS_GetStringBytes(JS_ValueToString(cx, argv[0]));


	text = JS_GetStringBytes(JS_ValueToString(cx, argv[0]));
#endif /* CLOWNS */

/* FIXME: I don't think the arg checking in this is right */
JSBool js_putlog(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	struct network *net = JS_GetContextPrivate(cx);
	char *text;

	/* Not that this will be used */	
	*rval = JSVAL_VOID;

	if (argc != 1)
	{
		/* Need to add error reporting */
		return JS_FALSE;
	}

	text = JS_GetStringBytes(JS_ValueToString(cx, argv[0]));
  
	egg_putlog(net, text);

	return JS_TRUE;
}

JSBool js_adduser(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	struct network *net = JS_GetContextPrivate(cx);
	char *username;
	char *hostmask;

	/* Not that this will be used */	
	*rval = JSVAL_VOID;

	/* channel */
	if (argc < 1 || argc > 2)
	{
		/* Need to add error reporting */
		return JS_FALSE;
	}

	username    = JS_GetStringBytes(JS_ValueToString(cx, argv[0]));
	hostmask    = (argc == 2) ? JS_GetStringBytes(JS_ValueToString(cx, argv[1])) : NULL;
  
	if (egg_adduser(net, username, hostmask) == 1)
	{
		return JS_TRUE;
	}
	else
	{
		return JS_FALSE;
	}
}

JSBool js_botisop(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	struct network *net = JS_GetContextPrivate(cx);
	char *channel;

	/* Not that this will be used */	
	*rval = JSVAL_VOID;

	/* channel */
	if (argc != 1)
	{
		/* Need to add error reporting */
		return JS_FALSE;
	}

	channel    = JS_GetStringBytes(JS_ValueToString(cx, argv[0]));
  
	if (egg_botisop(net, channel) == 1)
	{
		return JS_TRUE;
	}
	else
	{
		return JS_FALSE;
	}
}



JSBool js_unbind(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	struct network *net = JS_GetContextPrivate(cx);
	char *type;
	char *flags;
	char *mask;
	char *command;

	/* Not that this will be used */	
	*rval = JSVAL_VOID;

	/* type, flags, mask, command */
	if (argc != 4)
	{
		return JS_FALSE;
	}

	type    = JS_GetStringBytes(JS_ValueToString(cx, argv[0]));
	flags   = JS_GetStringBytes(JS_ValueToString(cx, argv[1]));
	mask    = JS_GetStringBytes(JS_ValueToString(cx, argv[2]));
	command = JS_GetStringBytes(JS_ValueToString(cx, argv[3]));
  
	egg_unbind(net, type, flags, mask, command);

	return JS_TRUE;
}


JSBool js_die(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	struct network *net = JS_GetContextPrivate(cx);
	char *reason;

	/* Not that this will be used */	
	*rval = JSVAL_VOID;

	if (argc == 0)
	{
		egg_die(net, NULL);
		return JS_TRUE;
	}

	if (argc > 0)
	{
		reason = JS_GetStringBytes(JS_ValueToString(cx, argv[0]));
	}

	egg_die(net, reason);

	return JS_TRUE;
}

JSBool js_isbansticky(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	struct network *net = JS_GetContextPrivate(cx);
	char *banmask;
	char *channel;
	int ret;

	if (argc == 1)
	{
		/* Just searching for the ban mask */
		banmask  = JS_GetStringBytes(JS_ValueToString(cx, argv[0]));
		channel  = NULL;
	}
	else if (argc == 2)
	{
		banmask  = JS_GetStringBytes(JS_ValueToString(cx, argv[0]));
		channel  = JS_GetStringBytes(JS_ValueToString(cx, argv[1]));
	}
	else
	{
		/* TODO: Error handling */
		return JS_FALSE;
	}

	ret = egg_isbansticky(net, banmask, channel);

	*rval = INT_TO_JSVAL(ret);

	return JS_TRUE;
}

JSBool js_ispermban(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	struct network *net = JS_GetContextPrivate(cx);
	char *banmask;
	char *channel;
	int ret;

	if (argc == 1)
	{
		/* Just searching for the ban mask */
		banmask  = JS_GetStringBytes(JS_ValueToString(cx, argv[0]));
		channel  = NULL;
	}
	else if (argc == 2)
	{
		banmask  = JS_GetStringBytes(JS_ValueToString(cx, argv[0]));
		channel  = JS_GetStringBytes(JS_ValueToString(cx, argv[1]));
	}
	else
	{
		/* TODO: Error handling */
		return JS_FALSE;
	}

	ret = egg_ispermban(net, banmask, channel);

	*rval = INT_TO_JSVAL(ret);

	return JS_TRUE;
}

JSBool js_isban(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	struct network *net = JS_GetContextPrivate(cx);
	char *banmask;
	char *channel;
	int ret;

	if (argc == 1)
	{
		/* Just searching for the ban mask */
		banmask  = JS_GetStringBytes(JS_ValueToString(cx, argv[0]));
		channel  = NULL;
	}
	else if (argc == 2)
	{
		banmask  = JS_GetStringBytes(JS_ValueToString(cx, argv[0]));
		channel  = JS_GetStringBytes(JS_ValueToString(cx, argv[1]));
	}
	else
	{
		/* TODO: Error handling */
		return JS_FALSE;
	}

	ret = egg_isban(net, banmask, channel);

	*rval = INT_TO_JSVAL(ret);

	return JS_TRUE;
}

JSBool js_isvoice(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	struct network *net = JS_GetContextPrivate(cx);
	char *nickname;
	char *channel;
	int ret;

	if (argc == 1)
	{
		/* Just searching for the nickname */
		nickname = JS_GetStringBytes(JS_ValueToString(cx, argv[0]));
		channel  = NULL;
	}
	else if (argc == 2)
	{
		nickname = JS_GetStringBytes(JS_ValueToString(cx, argv[0]));
		channel  = JS_GetStringBytes(JS_ValueToString(cx, argv[1]));
	}
	else
	{
		/* TODO: Error handling */
		return JS_FALSE;
	}

	ret = egg_isvoice(net, nickname, channel);

	*rval = INT_TO_JSVAL(ret);

	return JS_TRUE;
}

JSBool js_isop(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	struct network *net = JS_GetContextPrivate(cx);
	char *nickname;
	char *channel;
	int ret;

	if (argc == 1)
	{
		/* Just searching for the nickname */
		nickname = JS_GetStringBytes(JS_ValueToString(cx, argv[0]));
		channel  = NULL;
	}
	else if (argc == 2)
	{
		nickname = JS_GetStringBytes(JS_ValueToString(cx, argv[0]));
		channel  = JS_GetStringBytes(JS_ValueToString(cx, argv[1]));
	}
	else
	{
		/* TODO: Error Handling */
		return JS_FALSE;
	}

	ret = egg_isop(net, nickname, channel);

	*rval = INT_TO_JSVAL(ret);

	return JS_TRUE;
}

JSBool js_getchanmode(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	struct network *net = JS_GetContextPrivate(cx);
	char *ret;
	
	if (argc != 1)
		return JS_FALSE;

	ret = egg_getchanmode(net,JS_GetStringBytes(JS_ValueToString(cx, argv[0])));

	*rval = STRING_TO_JSVAL(JS_NewStringCopyZ(cx, ret));

	free(ret);

	return JS_TRUE;
}

JSBool js_getting_users(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	struct network *net = JS_GetContextPrivate(cx);
	int ret;

	ret = egg_getting_users(net);

	*rval = INT_TO_JSVAL(ret);

	return JS_TRUE;
}
	

JSBool js_dccbroadcast(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	struct network *net = JS_GetContextPrivate(cx);
	
	if (argc != 1)
		return JS_FALSE;
	
	egg_dccbroadcast(net, JS_GetStringBytes(JS_ValueToString(cx, argv[0])));

	*rval = JSVAL_VOID;

	return JS_TRUE;
}

JSBool js_putdcc(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	struct network *net = JS_GetContextPrivate(cx);
	int idx;
	
	if (argc != 2)
		return JS_FALSE;
	
	/* FIXME: Error checking */
	JS_ValueToInt32(cx, argv[0], &idx);

	egg_putdcc(net, idx, JS_GetStringBytes(JS_ValueToString(cx, argv[1])));

	*rval = JSVAL_VOID;

	return JS_TRUE;
}

JSBool js_isbotnick(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	struct network *net = JS_GetContextPrivate(cx);
	int ret;

	if (argc != 1)
		return JS_FALSE;

	ret = egg_isbotnick(net, JS_GetStringBytes(JS_ValueToString(cx, argv[0])));

	*rval = INT_TO_JSVAL(ret);

	return JS_TRUE;
}


JSBool js_encpass(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	char *ret;

	if (argc != 1)
		return JS_FALSE;

	ret = egg_encpass(JS_GetStringBytes(JS_ValueToString(cx, argv[0])));

	*rval = STRING_TO_JSVAL(JS_NewStringCopyZ(cx, ret));

	free(ret);

	return JS_TRUE;
}

JSBool js_topic(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	struct network *net = JS_GetContextPrivate(cx);
	char *ret;

	if (argc != 1)
		return JS_FALSE;

	ret = egg_topic(net, JS_GetStringBytes(JS_ValueToString(cx, argv[0])));

	*rval = STRING_TO_JSVAL(JS_NewStringCopyZ(cx, ret));

	free(ret);

	return JS_TRUE;
}

JSBool js_validuser(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	struct network *net = JS_GetContextPrivate(cx);
	int ret;

	if (argc != 1)
		return JS_FALSE;

	ret = egg_validuser(net, JS_GetStringBytes(JS_ValueToString(cx, argv[0])));

	*rval = INT_TO_JSVAL(ret);

	return JS_TRUE;
}

JSBool js_countusers(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	struct network *net = JS_GetContextPrivate(cx);
	int ret;

	ret = egg_countusers(net);

	*rval = INT_TO_JSVAL(ret);

	return JS_TRUE;
}

JSBool js_savechannels(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	struct network *net = JS_GetContextPrivate(cx);

	*rval = JSVAL_VOID;

	egg_savechannels(net);

	return JS_TRUE;
}

JSBool js_finduser(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	struct network *net  = JS_GetContextPrivate(cx);
	struct user    *user = NULL;
	char           *mask = NULL;

	if (argc != 1)
	{
		*rval = STRING_TO_JSVAL(JS_NewStringCopyZ(cx, "*"));
		return JS_TRUE;
	}

	mask = tstrdup(JS_GetStringBytes(JS_ValueToString(cx, argv[0])));

	user = egg_finduser(net,mask);

	if (user == NULL)
	{
		*rval = STRING_TO_JSVAL(JS_NewStringCopyZ(cx, "*"));
	}
	else
		*rval = STRING_TO_JSVAL(JS_NewStringCopyZ(cx, user->username));

	free(mask);

	return JS_TRUE;
}


JSBool js_matchattr(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	struct network *net = JS_GetContextPrivate(cx);
	char *handle  = NULL;
	char *flags   = NULL;
	char *channel = NULL;

	/* Not Enough Args */
	if (argc < 2)
	{
		*rval = INT_TO_JSVAL(0);
		return JS_FALSE;
	}

	handle  = tstrdup(JS_GetStringBytes(JS_ValueToString(cx, argv[0])));
	flags   = tstrdup(JS_GetStringBytes(JS_ValueToString(cx, argv[1])));

	if (argc == 3)
		channel = tstrdup(JS_GetStringBytes(JS_ValueToString(cx, argv[2])));

	*rval = INT_TO_JSVAL(egg_matchattr(net,handle,flags,channel));

	free(handle);
	free(flags);
	free(channel);

	return JS_TRUE;
}

JSBool js_save(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	struct network *net = JS_GetContextPrivate(cx);

	egg_save(net);

	*rval = JSVAL_VOID;

	return JS_TRUE;
}

/* Uh, no */
JSBool js_eval(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	JSString *jsc_script;
	JSString *jsc_chan;
	JSString *jsc_return;
	JSFunction *efunk;
	jsval our_rval;
	char *script = NULL;


	struct network *net = JS_GetContextPrivate(cx);

	jsc_script = JS_ValueToString(cx, argv[0]);
	jsc_chan   = JS_ValueToString(cx, argv[1]);

	script = tstrdup(JS_GetStringBytes(jsc_script));

	efunk = JS_CompileFunction(net->plain_cx, net->plain_global,
			"eval_func", 0, NULL, script, strlen(script),
			"Internal", 0);

	if ((JS_CallFunction(net->plain_cx, net->plain_global, efunk, 0, NULL, &our_rval)) == JS_TRUE)
	{
		jsc_return = JS_ValueToString(net->plain_cx, our_rval); 
		irc_printf(net->sock,"PRIVMSG %s :Javascript result: %s\n", JS_GetStringBytes(jsc_chan), JS_GetStringBytes(jsc_return));

		return JS_TRUE;
	}

	return JS_FALSE;
}


JSBool js_bind(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	char *type  = NULL;
	char *flags = NULL;
	char *mask  = NULL;
	char *func  = NULL;
	char **returnValue = NULL;
	struct network *net = JS_GetContextPrivate(cx);

	type = tstrdup(JS_GetStringBytes(JS_ValueToString(cx, argv[0])));
	flags = tstrdup(JS_GetStringBytes(JS_ValueToString(cx, argv[1])));
	mask = tstrdup(JS_GetStringBytes(JS_ValueToString(cx, argv[2])));

	if (!JSVAL_IS_NULL(argv[3]) && !JSVAL_IS_VOID(argv[3])){
		func = tstrdup(JS_GetStringBytes(JS_ValueToString(cx, argv[3])));
	}

	if (!type || !flags || !mask)
	{
		printf("Javascript error: Did not get all args for bind\n");
		free(type);
		free(flags);
		free(mask);
		free(func);

		return JS_FALSE;
	}

	returnValue = egg_bind(net,type,flags,mask,func,js_handler);
	if (returnValue == NULL)
	{
		JS_ReportError(cx, "Cannot perform bind operation.");
		return JS_FALSE;
	}
	else {
		if (func == NULL){
			int curIdx=0;
			JSObject *array;

			while (returnValue[curIdx] != NULL){ curIdx++; }

			array = JS_NewArrayObject(cx, curIdx, NULL);

			curIdx=0;
			while (returnValue[curIdx] != NULL){ 
				JSString *str = JS_NewStringCopyZ(cx, returnValue[curIdx]);
				JS_DefineElement(cx, array, curIdx, STRING_TO_JSVAL(str), NULL, NULL, 0);
				free(returnValue[curIdx++]);
			}
			free(returnValue);

			*rval = OBJECT_TO_JSVAL(array);
		}
		else {
			*rval = STRING_TO_JSVAL(JS_NewStringCopyZ(cx, returnValue[0]));
		}
		free(returnValue[0]);
		free(returnValue);
	}

	free(type);
	free(flags);
	free(mask);
	free(func);

	return JS_TRUE;
}

JSBool js_putserv(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	struct network *net = JS_GetContextPrivate(cx);
	JSString *str;

	str = JS_ValueToString(cx, argv[0]);

	irc_printf(net->sock,"%s",JS_GetStringBytes(str));

	*rval = JSVAL_VOID;

	return JS_TRUE;
}


JSBool js_botname(JSContext *cx, JSObject *obj, jsval id, jsval *rval)
{
	struct network *net = JS_GetContextPrivate(cx);
	char *botname;
	JSString *botnameJS;

	botname = egg_botname(net);
	botnameJS = JS_NewStringCopyZ(cx, botname);
	free(botname);
	*rval = STRING_TO_JSVAL(botnameJS);

	return JS_TRUE;
}

JSBool js_version(JSContext *cx, JSObject *obj, jsval id, jsval *rval)
{
	JSString *botversionJS;

	botversionJS = JS_NewStringCopyZ(cx, egg_version());
	*rval = STRING_TO_JSVAL(botversionJS);

	return JS_TRUE;
}



JSBool js_onchan(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	struct network *net = JS_GetContextPrivate(cx);
	char *nickToCheck=NULL;
	char *channelToCheck=NULL;

	nickToCheck = JS_GetStringBytes(JS_ValueToString(cx, argv[0]));
	if (argc == 2){
		if (JSVAL_IS_NULL(argv[1]) == JS_FALSE && JSVAL_IS_VOID(argv[1]) == JS_FALSE){
			channelToCheck = JS_GetStringBytes(JS_ValueToString(cx, argv[1]));
		}
	}

	*rval = INT_TO_JSVAL(egg_onchan(net, nickToCheck, channelToCheck));


	return JS_TRUE;
}

JSBool js_passwdok(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	struct network *net=JS_GetContextPrivate(cx);
	char *handle=NULL;
	char *pass=NULL;
	int ret=0;

	handle = JS_GetStringBytes(JS_ValueToString(cx, argv[0]));
	pass = JS_GetStringBytes(JS_ValueToString(cx, argv[1]));

	ret =egg_passwdok(net, handle, pass);

	*rval = INT_TO_JSVAL(ret);

	return JS_TRUE;
}

JSBool js_chhandle(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	struct network *net = JS_GetContextPrivate(cx);
	char *old=NULL;
	char *new=NULL;
	int ret=0;

	old = JS_GetStringBytes(JS_ValueToString(cx, argv[0]));
	new = JS_GetStringBytes(JS_ValueToString(cx, argv[1]));

	ret = egg_chhandle(net, old, new);

	*rval = INT_TO_JSVAL(ret);

	return JS_TRUE;

}

JSBool js_channels(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	struct network *net = JS_GetContextPrivate(cx);
	char *channels = egg_channels(net);

	*rval = STRING_TO_JSVAL(JS_NewStringCopyZ(cx, channels));

	free(channels);

	return JS_TRUE;

}

void js_handler(struct network *net, struct trigger *trig, struct irc_data *data, struct dcc_session *dcc, const char *dccbuf)
{
	jsval rval;
	jsval argv[10];

	if (net->global == NULL || net->cx == NULL)
	{
		printf("js_handler called with NULL context.\n");
		return;
	}

	switch (trig->type)
	{
		case TRIG_PUB:
			argv[0] = STRING_TO_JSVAL(JS_NewStringCopyZ(net->cx, data->prefix->nick));
			argv[1] = STRING_TO_JSVAL(JS_NewStringCopyZ(net->cx, data->prefix->host));
			argv[2]	= STRING_TO_JSVAL(JS_NewStringCopyZ(net->cx, "*"));
			argv[3] = STRING_TO_JSVAL(JS_NewStringCopyZ(net->cx, data->c_params[0]));
			argv[4] = STRING_TO_JSVAL(JS_NewStringCopyZ(net->cx, troll_makearg(data->rest_str,trig->mask)));

			JS_CallFunctionName(net->cx, net->global, trig->command, 5, argv, &rval);
			break;
		case TRIG_PUBM:
			argv[0] = STRING_TO_JSVAL(JS_NewStringCopyZ(net->cx, data->prefix->nick));
			argv[1] = STRING_TO_JSVAL(JS_NewStringCopyZ(net->cx, data->prefix->host));
			argv[2] = STRING_TO_JSVAL(JS_NewStringCopyZ(net->cx, "*"));
			argv[3] = STRING_TO_JSVAL(JS_NewStringCopyZ(net->cx, data->c_params[0]));
			argv[4] = STRING_TO_JSVAL(JS_NewStringCopyZ(net->cx, data->rest_str));

			JS_CallFunctionName(net->cx, net->global, trig->command, 5, argv, &rval);
			break;
		case TRIG_MSG:
			argv[0] = STRING_TO_JSVAL(JS_NewStringCopyZ(net->cx, data->prefix->nick));
			argv[1] = STRING_TO_JSVAL(JS_NewStringCopyZ(net->cx, data->prefix->host));
			argv[2] = STRING_TO_JSVAL(JS_NewStringCopyZ(net->cx, "*"));
			argv[3] = STRING_TO_JSVAL(JS_NewStringCopyZ(net->cx, troll_makearg(data->rest_str,trig->mask)));

			JS_CallFunctionName(net->cx, net->global, trig->command, 4, argv, &rval);

			break;
		case TRIG_MSGM:
			argv[0] = STRING_TO_JSVAL(JS_NewStringCopyZ(net->cx, data->prefix->nick));
			argv[1] = STRING_TO_JSVAL(JS_NewStringCopyZ(net->cx, data->prefix->host));
			argv[2] = STRING_TO_JSVAL(JS_NewStringCopyZ(net->cx, "*"));
			argv[3] = STRING_TO_JSVAL(JS_NewStringCopyZ(net->cx, data->rest_str));

			JS_CallFunctionName(net->cx, net->global, trig->command, 4, argv, &rval);

			break;
		case TRIG_TOPC:
			argv[0] = STRING_TO_JSVAL(JS_NewStringCopyZ(net->cx, data->prefix->nick));
			argv[1] = STRING_TO_JSVAL(JS_NewStringCopyZ(net->cx, data->prefix->host));
			argv[2] = STRING_TO_JSVAL(JS_NewStringCopyZ(net->cx, "*"));
			argv[3] = STRING_TO_JSVAL(JS_NewStringCopyZ(net->cx, data->c_params[0]));
			argv[4] = STRING_TO_JSVAL(JS_NewStringCopyZ(net->cx, data->rest_str));

			JS_CallFunctionName(net->cx, net->global, trig->command, 5, argv, &rval);
		case TRIG_JOIN:	
			argv[0] = STRING_TO_JSVAL(JS_NewStringCopyZ(net->cx, data->prefix->nick));
			argv[1] = STRING_TO_JSVAL(JS_NewStringCopyZ(net->cx, data->prefix->host));
			argv[2] = STRING_TO_JSVAL(JS_NewStringCopyZ(net->cx, "*"));
			argv[3] = STRING_TO_JSVAL(JS_NewStringCopyZ(net->cx, data->rest_str));
	
			JS_CallFunctionName(net->cx, net->global, trig->command, 4, argv, &rval);
			break;
		case TRIG_PART:
			argv[0] = STRING_TO_JSVAL(JS_NewStringCopyZ(net->cx, data->prefix->nick));
			argv[1] = STRING_TO_JSVAL(JS_NewStringCopyZ(net->cx, data->prefix->host));
			argv[2] = STRING_TO_JSVAL(JS_NewStringCopyZ(net->cx, "*"));
			argv[3] = STRING_TO_JSVAL(JS_NewStringCopyZ(net->cx, data->c_params[0]));
			argv[4] = STRING_TO_JSVAL(JS_NewStringCopyZ(net->cx, data->rest_str));

			JS_CallFunctionName(net->cx, net->global, trig->command, 5, argv, &rval);
			break;
		case TRIG_SIGN: /* Untested */
			argv[0] = STRING_TO_JSVAL(JS_NewStringCopyZ(net->cx, data->prefix->nick));
			argv[1] = STRING_TO_JSVAL(JS_NewStringCopyZ(net->cx, data->prefix->host));
			argv[2] = STRING_TO_JSVAL(JS_NewStringCopyZ(net->cx, "*"));
			argv[3] = STRING_TO_JSVAL(JS_NewStringCopyZ(net->cx, data->c_params[0]));
			argv[4] = STRING_TO_JSVAL(JS_NewStringCopyZ(net->cx, data->rest_str));

			JS_CallFunctionName(net->cx, net->global, trig->command, 5, argv, &rval);
			break;
		case TRIG_CTCP:
      argv[0] = STRING_TO_JSVAL(JS_NewStringCopyZ(net->cx, data->prefix->nick));
      argv[1] = STRING_TO_JSVAL(JS_NewStringCopyZ(net->cx, data->prefix->host));
      argv[2] = STRING_TO_JSVAL(JS_NewStringCopyZ(net->cx, "*"));
      argv[3] = STRING_TO_JSVAL(JS_NewStringCopyZ(net->cx, data->c_params[0]));
      argv[4] = STRING_TO_JSVAL(JS_NewStringCopyZ(net->cx, data->command));
			argv[5] = STRING_TO_JSVAL(JS_NewStringCopyZ(net->cx, data->rest_str));

      JS_CallFunctionName(net->cx, net->global, trig->command, 6, argv, &rval);
			break;
		case TRIG_DCC:
			argv[0] = STRING_TO_JSVAL(JS_NewStringCopyZ(net->cx, dcc->user->username));
			argv[1] = INT_TO_JSVAL(dcc->id);
			argv[2] = STRING_TO_JSVAL(JS_NewStringCopyZ(net->cx, dccbuf));
			break;
	}


}

