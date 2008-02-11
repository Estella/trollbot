#ifndef __JS_LIB_H__
#define __JS_LIB_H__

#include <jsapi.h>

struct trigger;
struct irc_data;
struct network;
struct dcc_session;

JSBool js_eval(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
JSBool js_bind(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
JSBool js_putserv(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
JSBool js_botname(JSContext *cx, JSObject *obj, jsval id, jsval *rval);
JSBool js_version(JSContext *cx, JSObject *obj, jsval id, jsval *rval);
JSBool js_onchan(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
void js_handler(struct network *net, struct trigger *trig, struct irc_data *data, struct dcc_session *dcc, const char *dccbuf);

#endif /* __JS_LIB_H__ */
