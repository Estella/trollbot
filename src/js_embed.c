#include <jsapi.h>

#include "main.h"
#include "js_embed.h"
#include "js_lib.h"

#include "network.h"

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

int js_eval_file(struct network *net, char *filename)
{
	JSScript *script =  JS_CompileFile(net->cx,net->global,filename);
  jsval rval;

	JS_ExecuteScript(net->cx, net->global, script, &rval);

	JS_DestroyScript(net->cx, script);
	return 1;	
}

void net_init_js(struct network *net)
{
	/* initialize the JS run time, and return result in rt */
  if (g_cfg->js_rt == NULL){
    g_cfg->js_rt = JS_NewRuntime(0x100000);

    /* if rt does not have a value, end the program here */
    if (g_cfg->js_rt == NULL)
      return;
  }

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

  /* initialize the built-in JS objects and the global object */
  builtins = JS_InitStandardClasses(net->cx, net->global);

	/* Initialize basic IRC I/O functions */
	JS_DefineFunction(net->cx, net->global, "bind", js_bind, 5, 0);

	JS_DefineFunction(net->cx, net->global, "putserv", js_putserv, 1, 0);

	/* So functions can access it */
	JS_SetContextPrivate(net->cx, net);
}


