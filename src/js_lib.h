/*********************************************
 * TrollBot v1.0                             *
 *********************************************
 * TrollBot is an eggdrop-clone designed to  *
 * work with multiple networks and protocols *
 * in order to present a unified scriptable  *
 * event-based platform,                     *
 *********************************************
 * This software is PUBLIC DOMAIN. Feel free *
 * to use it for whatever use whatsoever.    *
 *********************************************
 * Originally written by poutine/DALnet      *
 *                       kicken/DALnet       *
 *                       comcor/DALnet       *
 *********************************************/
#ifndef __JS_LIB_H__
#define __JS_LIB_H__

#include <jsapi.h>

struct trigger;
struct irc_data;
struct network;
struct dcc_session;

JSBool js_unstick(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
JSBool js_stick(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
JSBool js_deluser(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
JSBool js_stripcodes(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
JSBool js_hand2idx(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
JSBool js_putlog(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
JSBool js_adduser(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);

JSBool js_unbind(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
JSBool js_die(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);

JSBool js_isban(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
JSBool js_isbansticky(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
JSBool js_ispermban(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);



JSBool js_botisop(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
JSBool js_isop(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
JSBool js_isvoice(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);

JSBool js_getchanmode(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
JSBool js_getting_users(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
JSBool js_dccbroadcast(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
JSBool js_putdcc(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
JSBool js_isbotnick(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
JSBool js_encpass(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
JSBool js_topic(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
JSBool js_validuser(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
JSBool js_countusers(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
JSBool js_validuser(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
JSBool js_loadchannels(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
JSBool js_savechannels(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
JSBool js_finduser(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
JSBool js_matchattr(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
JSBool js_save(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
JSBool js_eval(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
JSBool js_bind(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
JSBool js_putserv(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
JSBool js_botname(JSContext *cx, JSObject *obj, jsval id, jsval *rval);
JSBool js_version(JSContext *cx, JSObject *obj, jsval id, jsval *rval);
JSBool js_onchan(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
JSBool js_passwdok(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
JSBool js_chhandle(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
JSBool js_channels(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
void js_handler(struct network *net, struct trigger *trig, struct irc_data *data, struct dcc_session *dcc, const char *dccbuf);

#endif /* __JS_LIB_H__ */
