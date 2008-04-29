#include <jsapi.h>

#include "main.h"
#include "js_embed.h"
#include "js_lib.h"

#include "network.h"
#include "trigger.h"
#include "irc.h"
#include "dcc.h"
#include "egg_lib.h"

JSVersion version;
JSBool builtins;

JSClass global_class = {
	"global",0,
	JS_PropertyStub,
	JS_PropertyStub,
	JS_PropertyStub,
	JS_PropertyStub,
	JS_EnumerateStub,
	JS_ResolveStub,
	JS_ConvertStub,
	JS_FinalizeStub
};

static void js_error_handler(JSContext *ctx, const char *msg, JSErrorReport *er);

void dcc_javascript(struct network *net, struct trigger *trig, struct irc_data *data, struct dcc_session *dcc, const char *dccbuf)
{
	jsval rval;

	if (JS_EvaluateScript(dcc->net->cx, 
				dcc->net->global, 
				egg_makearg(dccbuf,trig->mask), 
				strlen(egg_makearg(dccbuf,trig->mask)),
				"DCC",
				0,
				&rval) == JS_TRUE)
	{
		/* Error handling or anything ? */
	}
}

void js_load_scripts_from_config(struct config *cfg)
{
	int i;
	struct network *net = cfg->networks;

	while (net != NULL)
	{
		if (net->js_scripts != NULL)
		{
			if (net->cx == NULL) /* Should do proper state checking */
				net_init_js(net);

			for (i=0;net->js_scripts[i] != NULL;i++)
			{
				if (!js_eval_file(net,net->js_scripts[i]))
				{
					troll_debug(LOG_WARN, "Could not load Javascript file (%s)",net->js_scripts[i]);
				}
			}
		}

		net = net->next;
	}
}

void dcc_javascript_load(struct network *net, struct trigger *trig, struct irc_data *data, struct dcc_session *dcc, const char *dccbuf)
{
	if (!strcmp(trig->mask,dccbuf))
		return;

	js_eval_file(net, egg_makearg(dccbuf,trig->mask));

	irc_printf(dcc->sock, "Loaded Javascript file: %s",egg_makearg(dccbuf,trig->mask));

	return;
}

int js_eval_file(struct network *net, char *filename)
{
	JSScript *script =  JS_CompileFile(net->cx,net->global,filename);
	jsval rval;

	if (script == NULL){
		return 0;
	}

	JS_ExecuteScript(net->cx, net->global, script, &rval);

	JS_DestroyScript(net->cx, script);
	return 1;	
}

void net_init_js(struct network *net)
{
	/* Check if happening multiple times */
	if (g_cfg->js_rt == NULL)
	{
		/* initialize the JS run time, and return result in rt */
		g_cfg->js_rt = JS_NewRuntime(0x50000);

		/* if rt does not have a value, end the javascript portion here */
		if (g_cfg->js_rt == NULL)
			return;
	}

	/* create a context and associate it with the JS run time */
	net->cx = JS_NewContext(g_cfg->js_rt, 8192);

	/* if cx does not have a value, end the javascript portion here */
	if (net->cx == NULL) 
	{
		net->cx = NULL;
		return;
	}

	JS_SetErrorReporter(net->cx, js_error_handler);
	net_init_js_global_object(net);

	/* We should free context and global runtime here */
	if (net->global == NULL)
	{
		troll_debug(LOG_ERROR,"net_init_js_global_object() failed");
		return;
	}

	return;
}

static void js_error_handler(JSContext *ctx, const char *msg, JSErrorReport *er){
	char *pointer=NULL;
	char *line=NULL;
	int len;

	if (er->linebuf != NULL){
		line = tstrdup(er->linebuf);
		len = er->tokenptr - er->linebuf + 2;
		pointer = malloc(len);
		memset(pointer, '-', len-2);
		pointer[len-1]='\0';
		pointer[len-2]='^';
	}
	else {
		len=0;
		pointer = malloc(1);
		line = malloc(1);
		pointer[0]='\0';
		line[0] = '\0';
	}

	while (len > 0){
		if (line[len-1] == '\r' || line[len-1] == '\n'){
			line[len-1]='\0';
		}
		else if (line[len-1]=='\t'){
			/*Convert tabs into spaces */
			line[len-1]=' ';
		}
		len--;
	}



	printf("JS Error: %s\nFile: %s:%u\n", msg, er->filename, er->lineno);

	if (line[0]){
		printf("%s\n%s\n", line, pointer);
	}

	free(pointer);
	free(line);
}

void net_init_js_global_object(struct network *net)
{
	if (net->cx == NULL || g_cfg->js_rt == NULL)
	{
		troll_debug(LOG_ERROR,"Javascript context or global runtime is NULL and net_init_js_global_object() was called");
		return;
	}

	/* create the global object here */
	net->global = JS_NewObject(net->cx, &global_class, NULL, NULL);

	/* initialize the built-in JS objects and the global object */
	/* builtins is a global JSBool */
	builtins = JS_InitStandardClasses(net->cx, net->global);

	/* Initialize egg_lib functions */
	JS_DefineFunction(net->cx, net->global, "validuser", js_validuser, 1, 0);

	JS_DefineFunction(net->cx, net->global, "countusers", js_countusers, 0, 0);

	JS_DefineFunction(net->cx, net->global, "savechannels", js_savechannels, 0, 0);

	JS_DefineFunction(net->cx, net->global, "finduser", js_finduser, 1, 0);

	JS_DefineFunction(net->cx, net->global, "matchattr", js_matchattr, 3, 0);

	JS_DefineFunction(net->cx, net->global, "bind", js_bind, 5, 0);

	JS_DefineFunction(net->cx, net->global, "putserv", js_putserv, 1, 0);

	/* JS_DefineFunction(net->cx, net->global, "js_eval", js_eval, 1, 0); */

	JS_DefineProperty(net->cx, net->global, "botname", JSVAL_VOID, js_botname, NULL, 0);

	JS_DefineProperty(net->cx, net->global, "version", JSVAL_VOID, js_version, NULL, 0);

	JS_DefineFunction(net->cx, net->global, "onchan", js_onchan, 5, 0);

	JS_DefineFunction(net->cx, net->global, "validuser", js_validuser, 1, 0);

	JS_DefineFunction(net->cx, net->global, "passwdok", js_passwdok, 2, 0);

	JS_DefineFunction(net->cx, net->global, "chhandle", js_chhandle, 2, 0);

	JS_DefineFunction(net->cx, net->global, "channels", js_channels, 2, 0);

	JS_DefineFunction(net->cx, net->global, "save", js_save, 0, 0);

	/* Store a pointer to the net struct in the Javascript context */
	JS_SetContextPrivate(net->cx, net);
}


