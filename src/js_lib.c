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


JSBool js_bind(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
  jsval str;
  char *type  = NULL;
  char *flags = NULL;
  char *mask  = NULL;
  char *func  = NULL;
	int curarg;
	struct network *net = JS_GetContextPrivate(cx);

	for(curarg=0;curarg<argc;curarg++)
	{
		str = JS_ValueToString(cx, argv[curarg]);

		switch (curarg)
		{
			case 0:
				type = tstrdup(JS_GetStringBytes(str));
				break;
			case 1:
				flags = tstrdup(JS_GetStringBytes(str));
				break;
			case 2:
				mask = tstrdup(JS_GetStringBytes(str));
				break;
			case 3:
				func = tstrdup(JS_GetStringBytes(str));
				break;
		}
	}

	if (!type || !flags || !mask || !func)
	{
		printf("Javascript error: Did not get all args for bind\n");
		free(type);
		free(flags);
		free(mask);
		free(func);

		return JS_FALSE;
	}

	if (!egg_bind(net,type,flags,mask,func,js_handler))
	{
		return JS_FALSE;
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
	jsval str;

	str = JS_ValueToString(cx, argv[0]);

	irc_printf(net->sock,"%s",JS_StringGetBytes(str));

	return 1;
}

void js_handler(struct network *net, struct trigger *trig, struct irc_data *data, struct dcc_session *dcc, const char *dccbuf)
{
	jsval rval;
	jsval argv[10];

  switch (trig->type)
  {
    case TRIG_PUB:
			argv[0] = JS_NewStringCopyZ(net->cx, data->prefix->nick);
			argv[1] = JS_NewStringCopyZ(net->cx, data->prefix->host);
			argv[2]	= JS_NewStringCopyZ(net->cx, "*");
      argv[3] = JS_NewStringCopyZ(net->cx, data->c_params[0]);
			argv[4] = JS_NewStringCopyZ(net->cx, data->rest_str);

			JS_CallFunctionName(net->cx, net->global, trig->command, 5, argv, &rval);
			break;
		case TRIG_PUBM:
			break;
		case TRIG_MSG:
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

