#ifndef __TROLL_LIB_H__
#define __TROLL_LIB_H__


struct network;
struct trigger;
struct irc_data;
struct dcc_session;

void troll_parse_who(struct network *net, struct trigger *trig, struct irc_data *data, struct dcc_session *dcc, const char *dccbuf);
void troll_trig_update_ban(struct network *net, struct trigger *trig, struct irc_data *data, struct dcc_session *dcc, const char *dccbuf);
void troll_trig_update_topic(struct network *net, struct trigger *trig, struct irc_data *data, struct dcc_session *dcc, const char *dccbuf);

#endif /* __TROLL_LIB_H__ */
