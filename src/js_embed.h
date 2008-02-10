#ifndef __JS_EMBED_H__
#define __JS_EMBED_H__

struct network;
struct trigger;
struct irc_data;
struct dcc_session;

void dcc_javascript_load(struct network *net, struct trigger *trig, struct irc_data *data, struct dcc_session *dcc, const char *dccbuf);
int js_eval_file(struct network *net, char *filename);
void net_init_js(struct network *net);

#endif /* __JS_EMBED_H__ */
