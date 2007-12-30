#ifndef __TCL_EMBED_H__
#define __TCL_EMBED_H__

struct network;
struct trigger;
struct irc_data;

void net_init_tcl(struct network *net);
void tcl_handler(struct network *net, struct trigger *trig, struct irc_data *data, struct dcc_session *dcc, const char *dccbuf);

#endif /* __TCL_EMBED_H__ */
