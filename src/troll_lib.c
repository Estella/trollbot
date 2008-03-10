#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "troll_lib.h"
#include "util.h"

#include "irc.h"
#include "network.h"
#include "channel.h"
#include "log_entry.h"
#include "debug.h"

void troll_trig_update_ban(struct network *net, struct trigger *trig, struct irc_data *data, struct dcc_session *dcc, const char *dccbuf)
{
}

/*
 * [00:19] Nick: poutine
 * [00:19] User: ben
 * [00:19] Host: i.love.debian.org
 * [00:19] Command: TOPIC
 * [00:19] Command Parameters: #test
 * [00:19] Rest: weee I eat poo
 */
void troll_trig_update_topic(struct network *net, struct trigger *trig, struct irc_data *data, struct dcc_session *dcc, const char *dccbuf)
{
	struct channel *chan = NULL;

	chan = net->chans;

	/* TODO: Ensure not needed */
	if (chan != NULL)
		while (chan->prev != NULL)
			chan = chan->prev;

	while (chan != NULL)
	{
		if (!tstrcasecmp(data->c_params[0],chan->name))
			break;

		chan = chan->next;
	}

	if (chan == NULL)
	{
		log_entry_printf(net,NULL,"T","Add eggdrop log flag entry for this event, also handle a NULL channel gracefully.");
		return;
	}

	/* It's okay if it's NULL */
	free(chan->topic);

	chan->topic = tstrdup(data->rest_str);
	log_entry_printf(net,chan,"T","Topic changed for channel %s to :%s",data->c_params[0],data->rest_str);

	return;
}
