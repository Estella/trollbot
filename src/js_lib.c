#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <jsapi.h>

#include "js_lib.h"
#include "js_embed.h"
#include "trigger.h"	
#include "util.h"
#include "user.h"
#include "dcc.h"
#include "irc.h"
#include "trigger.h"
#include "network.h"
#include "egg_lib.h"

JSBool js_matchattr(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
  struct network *net = JS_GetContextPrivate(cx);
	int ret;
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

	return JS_TRUE;
}

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
			argv[4] = STRING_TO_JSVAL(JS_NewStringCopyZ(net->cx, egg_makearg(data->rest_str,trig->mask)));

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
      argv[3] = STRING_TO_JSVAL(JS_NewStringCopyZ(net->cx, egg_makearg(data->rest_str,trig->mask)));

      JS_CallFunctionName(net->cx, net->global, trig->command, 4, argv, &rval);

			break;
		case TRIG_MSGM:
      argv[0] = STRING_TO_JSVAL(JS_NewStringCopyZ(net->cx, data->prefix->nick));
      argv[1] = STRING_TO_JSVAL(JS_NewStringCopyZ(net->cx, data->prefix->host));
      argv[2] = STRING_TO_JSVAL(JS_NewStringCopyZ(net->cx, "*"));
      argv[3] = STRING_TO_JSVAL(JS_NewStringCopyZ(net->cx, data->rest_str));

      JS_CallFunctionName(net->cx, net->global, trig->command, 4, argv, &rval);

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

