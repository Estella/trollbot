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

	/* Insert it into the list */
	if (chan->user_list == NULL)
		slist_init(&chan->user_list, free_channel_user);

	slist_insert_next(chan->user_list, NULL, (void *)cuser);
}

/*
 * [20:41] Nick: poutine
 * [20:41] User: poutine
 * [20:41] Host: 192.168.41.101
 * [20:41] Command: MODE
 * [20:41] Command Parameters: #test +bb *!*@* foo*!*@*
 */
/* This might get ugly */
void troll_trig_update_mode(struct network *net, struct trigger *trig, struct irc_data *data, struct dcc_session *dcc, const char *dccbuf)
{
	struct channel      *chan  = NULL;
	struct channel_user *cuser = NULL;
	struct channel_ban  *cban  = NULL;

	char *newchanmode;
	char *tmpptr;

	int mode = 1; /* 0 = -, 1 = + */
	int arg_index = 0;
	unsigned int oldmodesize = 0;
	
	if ((chan = network_channel_find(net, data->c_params[0])) == NULL)
		return;

	newchanmode = data->c_params[1];

	while (*newchanmode != '\0')
	{
		switch (*newchanmode)
		{
			case '+':
				mode = 1;
				break;
			case '-':
				mode = 0;
				break;
			case 'o':
				/* Ops [user] HAS_ARGS */
				cuser = channel_channel_user_find(chan, data->c_params[arg_index+2]);

				if (mode == 1)
				{
					/* FIXME: Bounds checking */
					/* data->c_params[arg_index+2] == nick */
						
					if (cuser != NULL)
					{
						if (cuser->modes == NULL)
						{
							cuser->modes = tstrdup("o");
						} 
						else 
						{
							if (strchr(cuser->modes,'o') == NULL)
							{
								/* First try and find a space */	
								if ((tmpptr = strchr(cuser->modes,' ')) == NULL)
								{
									/* No Spaces, resize */
									oldmodesize = strlen(cuser->modes);
									cuser->modes = tcrealloc0(cuser->modes, strlen(cuser->modes) + 2, &oldmodesize); 
									cuser->modes[strlen(cuser->modes)] = 'o';
								}
								else
									*tmpptr ='o';
							}			
						}
					}
				}
				else
				{
					if (cuser != NULL)
						if (cuser->modes != NULL)
							if ((tmpptr = strchr(cuser->modes,'o')) != NULL)
								*tmpptr = ' ';
				}
	
				arg_index++;

				break;
			case 'v':
				/* Voice [user] HAS_ARGS */
				cuser = channel_channel_user_find(chan, data->c_params[arg_index+2]);

				if (mode == 1)
				{
					/* FIXME: Bounds checking */
					/* data->c_params[arg_index+2] == nick */
					if (cuser != NULL)
					{
						if (cuser->modes == NULL)
						{
							cuser->modes = tstrdup("v");
						} 
						else 
						{
							if (strchr(cuser->modes,'v') == NULL)
							{
								/* First try and find a space */	
								if ((tmpptr = strchr(cuser->modes,' ')) == NULL)
								{
									/* No Spaces, resize */
									oldmodesize = strlen(cuser->modes);
									cuser->modes = tcrealloc0(cuser->modes, strlen(cuser->modes) + 2, &oldmodesize); 
									cuser->modes[strlen(cuser->modes)] = 'v';
								}
								else
									*tmpptr ='v';
							}			
						}
					}
				}
				else
				{
					if (cuser != NULL)
						if (cuser->modes != NULL)
							if ((tmpptr = strchr(cuser->modes,'v')) != NULL)
								*tmpptr = ' ';
				}
	
				arg_index++;

				break;
			case 'k':
				/* Channel key [chan] HAS_ARGS */
				break;
			case 'e':	
				/* exempt [chan] maybe user too HAS_ARGS */
				break;
			case 'I':	
				/* invite [user] HAS_ARGS */
				break;
			case 'c':
				/* No Colors (no bolds, reverses on DALnet also NEED_DETECT) */
				break;
			case 'b':
				/* ban [chan] maybe user too HAS_ARGS */
				if (mode == 1)
				{
					/* FIXME: Bounds checking */
					/* data->c_params[arg_index+2] == mask */
					cban = channel_ban_new();
					cban->chan = tstrdup(data->c_params[0]);
					cban->mask = tstrdup(data->c_params[arg_index+2]);
					cban->who  = tstrdup(data->prefix->nick);
					arg_index++; /* So the next thing recognizes this bitch */
					cban->time = time(NULL);
					
					chan->banlist = channel_ban_add(chan->banlist, cban);
				}
				else
				{
					/* Find if it exists */
					cban = chan->banlist;

					while (cban != NULL)
					{
						if (!tstrcasecmp(cban->mask, data->c_params[arg_index+2]))
						{
							chan->banlist = channel_ban_del(chan->banlist, cban);
							channel_ban_free(cban);
							break;
						}

						cban = cban->next;
					}
	
					arg_index++;
				}
				break;
			case 'j':
				/* join limit [chan] HAS_ARGS */
				break;
			default:
				/* assume chan, no args */
				break;
		}

		newchanmode++;
	}

	return;
}

/*
 * [20:28] Nick: poutine
 * [20:28] User: poutine
 * [20:28] Host: 192.168.41.101
 * [20:28] Command: NICK
 * [20:28] Rest: newnick
 */
void troll_trig_update_nick(struct network *net, struct trigger *trig, struct irc_data *data, struct dcc_session *dcc, const char *dccbuf)
{
	struct channel      *chan  = NULL;
	struct channel_user *cuser = NULL;

	chan = net->chans;

	while (chan != NULL)
	{
		cuser = channel_channel_user_find(chan, data->prefix->nick);
	
		if (cuser != NULL)
		{
			free(cuser->nick);
			cuser->nick = tstrdup(data->rest[0]);
		}

		chan = chan->next;
	}

	return;
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
