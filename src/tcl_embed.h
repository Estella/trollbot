#ifndef __TCL_EMBED_H__
#define __TCL_EMBED_H__

#include "irc.h"
#include "trigger.h"

void net_init_tcl(struct network *net);
void tcl_handler(struct network *net, struct trigger *trig, struct irc_data *data);

#endif /* __TCL_EMBED_H__ */
