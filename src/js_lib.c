#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <jsapi.h>

#include "js_lib.h"

JSBool js_bind(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
  jsval str;

  str = JS_ValueToString(cx, *argv);
  printf("Javascript function putserv passed: %s\n", JS_GetStringBytes(str));

	return 1;
}

JSBool js_putserv(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	jsval str;

  str = JS_ValueToString(cx, *argv);
  printf("Javascript function putserv passed: %s\n", JS_GetStringBytes(str));

	return 1;
}

