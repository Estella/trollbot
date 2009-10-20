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
#ifndef __JS_EMBED_H__
#define __JS_EMBED_H__

struct network;
struct trigger;
struct irc_data;
struct dcc_session;
struct config;

void dcc_javascript(struct network *net, struct trigger *trig, struct irc_data *data, struct dcc_session *dcc, const char *dccbuf);
void js_load_scripts_from_config(struct config *cfg);
void dcc_javascript_load(struct network *net, struct trigger *trig, struct irc_data *data, struct dcc_session *dcc, const char *dccbuf);
int js_eval_file(struct network *net, char *filename);
void net_init_js(struct network *net);
void net_init_js_global_object(struct network *net);

#endif /* __JS_EMBED_H__ */
