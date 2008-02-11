#include <jsapi.h>

#include "main.h"
#include "js_embed.h"
#include "js_lib.h"

#include "network.h"
#include "trigger.h"
#include "irc.h"
#include "dcc.h"

JSVersion version;
JSBool builtins;

JSClass plain_global_class = {
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
	if (data->rest_str == NULL)
		return;

	js_eval_file(net, data->rest_str);

	irc_printf(dcc->sock, "Loaded Javascript file: %s\n",data->rest_str);

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
	JSRuntime *rt, *plain_rt;

	/* initialize the JS run time, and return result in rt */
  rt = JS_NewRuntime(0x50000);
	plain_rt = JS_NewRuntime(0x50000);

  if (g_cfg->js_rt == NULL){
    g_cfg->js_rt = JS_NewRuntime(0x100000);

    /* if rt does not have a value, end the program here */
    if (g_cfg->js_rt == NULL)
      return;
  }

	if (plain_rt)
		net->plain_cx = JS_NewContext(plain_rt, 8192);

  /* create a context and associate it with the JS run time */
  net->cx = JS_NewContext(g_cfg->js_rt, 8192);

  /* if cx does not have a value, end the program here */
  if (net->cx == NULL) 
	{
		net->cx = NULL;
    return;
	}

  /* create the global object here */
  net->global = JS_NewObject(net->cx, &global_class, NULL, NULL);

	if (net->plain_cx)
	{
		/* Create a plain javascript global object */
		net->plain_global = JS_NewObject(net->plain_cx, &plain_global_class, NULL, NULL);

		builtins = JS_InitStandardClasses(net->plain_cx, net->plain_global);
	
		JS_SetContextPrivate(net->plain_cx, net);
	
		printf("SHould have plain cx\n");
	}

  /* initialize the built-in JS objects and the global object */
  builtins = JS_InitStandardClasses(net->cx, net->global);

	/* Initialize basic IRC I/O functions */
	JS_DefineFunction(net->cx, net->global, "bind", js_bind, 5, 0);

	JS_DefineFunction(net->cx, net->global, "putserv", js_putserv, 1, 0);

	JS_DefineFunction(net->cx, net->global, "js_eval", js_eval, 1, 0);

	JS_DefineProperty(net->cx, net->global, "botname", JSVAL_VOID, js_botname, NULL, 0);

	JS_DefineProperty(net->cx, net->global, "version", JSVAL_VOID, js_version, NULL, 0);

	JS_DefineFunction(net->cx, net->global, "onchan", js_onchan, 5, 0);

	/* So functions can access it */
	JS_SetContextPrivate(net->cx, net);
}


