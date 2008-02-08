#ifndef __JS_LIB_H__
#define __JS_LIB_H__

#include <jsapi.h>

JSBool js_bind(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
JSBool js_putserv(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);

#endif /* __JS_LIB_H__ */
