#ifndef __DEFAULT_TRIGGERS_H__
#define __DEFAULT_TRIGGERS_H__

#include "irc.h"
#include "network.h"
#include "trigger.h"

void add_default_triggers(void);

void new_join(struct network *net, struct trigger *trig, struct irc_data *data);
void new_part(struct network *net, struct trigger *trig, struct irc_data *data);
void new_quit(struct network *net, struct trigger *trig, struct irc_data *data);
void new_kick(struct network *net, struct trigger *trig, struct irc_data *data);
void new_user_pass(struct network *net, struct trigger *trig, struct irc_data *data);
void check_user_pass(struct network *net, struct trigger *trig, struct irc_data *data);
void introduce_user(struct network *net, struct trigger *trig, struct irc_data *data);
void return_ctcp_ping(struct network *net, struct trigger *trig, struct irc_data *data);
void return_ctcp_time(struct network *net, struct trigger *trig, struct irc_data *data);
void return_ctcp_version(struct network *net, struct trigger *trig, struct irc_data *data);


#endif /* __DEFAULT_TRIGGERS_H__ */
