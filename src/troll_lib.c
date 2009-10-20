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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <time.h>


#include "troll_lib.h"
#include "util.h"

#include "irc.h"
#include "irc_network.h"
#include "irc_channel.h"
#include "log_entry.h"
#include "debug.h"

/* 
	 ?  matches any single character
   *  matches 0 or more characters of any type
   %  matches 0 or more non-space characters (can be used to match a single
      word)
   ~  matches 1 or more space characters (can be used for whitespace between
      words)
	 \  makes the next character literal

   returns 1 if no match, 0 if matched
*/
int troll_matchwilds(const char *haystack, const char *needle)
{
	int escaped = 0;

	if (needle == NULL || haystack == NULL)
		return 1;

	while (*needle)
	{
		if (*haystack == '\0')
		{
			/* If *needle is '*', and *(needle+1) = '\0', this should return 0 for success */
			if ((*needle == '*') && *(needle+1) == '\0' && escaped == 0)
				return 0;
			/* Hit end of haystack but not the ned of needle, so match fails. */
			return 1;
		}

		if (*needle == '\\')
		{
			escaped = 1;
			needle++;
		}

		if (*needle == '?' && escaped == 0)
		{
			/* Any character matches, just move on. */
			needle++;
			haystack++;
		}
		else if (*needle == '*' && escaped == 0)
		{
			/* Match characters til end of haystack, or until *(needle+1) */
			while (*haystack != '\0' && *haystack != *(needle+1))
			{
				haystack++;
			}
			needle++;
		}
		else if (*needle == '%' && escaped == 0)
		{
			while (*haystack != '\0' && !isspace(*haystack) && *haystack != *(needle+1))
			{
				haystack++;
			}
			needle++;
		}
		else if (*needle == '~' && escaped == 0)
		{
			if (isspace(*haystack))
			{
				haystack++;
				while (*haystack != '\0' && isspace(*haystack))
				{
					haystack++;
				}
				needle++;
			}
			else 
			{
				/* Must match at least one space. */
				return 1;
			}
		}
		else if (*needle != *haystack)
		{
			return 1;
		}
		else 
		{
			/* Two characters match.  Next */
			needle++;
			haystack++;
	
			escaped = 0;
		}
	}

	if (*haystack == '\0')
	{
		/* Hit end of haystack and end of needle, so match succeeded */
		return 0;
	}
	else 
	{
		/* Hit end of needled, but not end of haystack, match fails. */
		return 1;
	}
}

char *troll_makearg(const char *rest, const char *mask)
{
	char *ret;

	ret = (char*)(&rest[strlen(mask)]);

	while (*ret != '\0' && (*ret == ' ' || *ret == '\t'))
		ret++;

	if (ret == NULL)
		return "";

	return ret; 
}

void troll_user_host_handler(struct network *net, struct trigger *trig, struct irc_data *data, struct dcc_session *dcc, const char *dccbuf)
{
	irc_printf(net->sock,"USERHOST %s",net->nick);
	join_channels(net);
	net->status = NET_IDLE;
}

void troll_nick_in_use_handler(struct network *net, struct trigger *trig, struct irc_data *data, struct dcc_session *dcc, const char *dccbuf)
{
	if (net->altnick != NULL)
		irc_printf(net->sock,"NICK %s",net->altnick);

	if (net->botnick != NULL)
		free(net->botnick);

	/* Should just make this a pointer to nick
	 * or altnick
	 */
	net->botnick = tstrdup(net->altnick);

	irc_printf(net->sock, "NICK %s", net->botnick);

	return;
}

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
	chan->user_list = channel_user_add(chan->user_list, cuser);
}

/* [00:14] Command: NOTICE
 * [00:14] Command Parameters: AUTH
 * [00:14] Rest: *** Ident is broken or disabled, to continue to connect you must type /QUOTE PASS 11022
 */
void troll_undernet_hack(struct network *net, struct trigger *trig, struct irc_data *data, struct dcc_session *dcc, const char *dccbuf)
{
	int i;

	if (data->c_params == NULL)
		return;
	
	if (!tstrcasecmp(data->c_params[0], "AUTH"))
	{
		for (i=0; data->rest[i] != NULL; i++)
		{
			if (!tstrcasecmp(data->rest[i], "/QUOTE"))
				break;
		}

		if (data->rest[i] == NULL)
			return;

		if (data->rest[i+1] == NULL)
			return;

		if (data->rest[i+2] == NULL)
			return;

		irc_printf(net->sock, "%s %s", data->rest[i+1], data->rest[i+2]);
		
		troll_debug(LOG_DEBUG, "Undernet's weak bot detection fooled. Alan Turing rolling in his grave.");
	}
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
	int cnt = 0; /* Counter for bounds checking */
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
				for (cnt = 0; cnt < (arg_index+2);cnt++)
					if (data->c_params[cnt] == NULL)
						return;
				cuser = channel_channel_user_find(chan, data->c_params[arg_index+2]);

				if (mode == 1)
				{
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
				for (cnt = 0; cnt < (arg_index+2);cnt++)
					if (data->c_params[cnt] == NULL)
						return;

				cuser = channel_channel_user_find(chan, data->c_params[arg_index+2]);

				if (mode == 1)
				{
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
					for (cnt = 0; cnt < (arg_index+2);cnt++)
						if (data->c_params[cnt] == NULL)
							return;

					/* data->c_params[arg_index+2] == mask */
					cban = channel_ban_new();
					cban->chan = tstrdup(data->c_params[0]);
					cban->mask = tstrdup(data->c_params[arg_index+2]);
					cban->who  = tmalloc0(strlen(data->prefix->nick) + strlen(data->prefix->user) + strlen(data->prefix->host) + 3);
					sprintf(cban->who, "%s!%s@%s",data->prefix->nick, data->prefix->user, data->prefix->host);
					arg_index++; /* So the next thing recognizes this bitch */
					cban->time = time(NULL);
					
					chan->banlist = channel_ban_add(chan->banlist, cban);
				}
				else
				{
					/* Find if it exists */
					for (cnt = 0; cnt < (arg_index+2);cnt++)
						if (data->c_params[cnt] == NULL)
							return;

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

/* Channel redirection a la freenode */
void troll_trig_channel_redirect(struct network *net, struct trigger *trig, struct irc_data *data, struct dcc_session *dcc, const char *dccbuf)
{
	struct channel *chan;

	/*       servername         cmd cparam1 cparam2      cparam3        rest_str                      */
	/* >> :kubrick.freenode.net 470 poutine #electronics ##electronics :Forwarding to another channel */

	/* Change all internal references to the new channel */
	chan = network_channel_find(net, data->c_params[2]);

	if (chan == NULL)
		return;

	free(chan->name);

	chan->name = tstrdup(data->c_params[3]);

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
	log_entry_printf(net,chan->name,"T","Topic changed for channel %s to :%s",data->c_params[0],data->rest_str);

	return;
}
