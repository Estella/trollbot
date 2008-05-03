#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <time.h>


#include "troll_lib.h"
#include "util.h"

#include "irc.h"
#include "network.h"
#include "channel.h"
#include "log_entry.h"
#include "debug.h"

/*                            C    CP0    CP1   CP2                     CP3                       CP4             CP5  CP6  REST    */
/* >> :swiftco.wa.us.dal.net 352 poutine #php ~address c-69-251-232-134.hsd1.md.comcast.net jade.fl.us.dal.net Wildblue H :5 yonder */
void troll_parse_who(struct network *net, struct trigger *trig, struct irc_data *data, struct dcc_session *dcc, const char *dccbuf)
{
	struct channel      *chan  = network_channel_find(net,data->c_params[1]);
	struct channel_user *cuser = NULL;

	if (chan == NULL)
	{
		log_entry_printf(net,NULL,"T","Could not find channel record in troll_parse_who() for %s",data->c_params[1]);
		return;
	}

	cuser = channel_channel_user_find(chan, data->c_params[5]);

	if (cuser != NULL)
	{
		/* Update the user's info */
		/* Try linking channel user to trollbot user */
		return;
	}

	/* The jointime isn't right, but that info is not available */
	cuser = new_channel_user();

	cuser->nick  = tstrdup(data->c_params[5]);
	cuser->ident = tstrdup(data->c_params[2]);
	

	cuser->jointime = time(NULL);

	cuser->ident = tstrdup(data->c_params[2]);
	cuser->modes = NULL;
	cuser->host  = tstrdup(data->c_params[3]);
}

void troll_trig_update_ban(struct network *net, struct trigger *trig, struct irc_data *data, struct dcc_session *dcc, const char *dccbuf)
{
	/* I need to parse out this motherfucker */
	/* Needs to deal with stackables */
	/* The modes dealt with here are probably network specific */
	/* I should be able to deal with this in an easy to use and update format,
 	 * Most of these behaviors are detectable, so I should be able to update trollbot's
 	 * knowledge of the network */

}

/*
 * [00:19] Nick: poutine
 * [00:19] User: ben
 * [00:19] Host: i.love.debian.org
 * [00:19] Command: TOPIC
 * [00:19] Command Parameters: #test
 * [00:19] Rest: test
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
