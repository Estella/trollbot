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
	JSString *str = NULL;
	jsval rval;

	JS_ExecuteScript(net->cx, net->global, script, &rval);

	str = JS_ValueToString(net->cx, rval);
	printf("Javascript result: %s\n", JS_GetStringBytes(str));

	/* FIXME: Freeing script memory anyone? */
	return 1;	
}

void net_init_js(struct network *net)
{
	JSRuntime *rt;
	/* initialize the JS run time, and return result in rt */
  rt = JS_NewRuntime(0x100000);

  /* if rt does not have a value, end the program here */
  if (!rt)
    return;

  /* create a context and associate it with the JS run time */
  net->cx = JS_NewContext(rt, 8192);

  /* if cx does not have a value, end the program here */
  if (net->cx == NULL) /* FIXME: Free memory for rt? */
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


