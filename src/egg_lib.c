#include <stdio.h>
#include <ctype.h>

#include <sys/socket.h>

#include "main.h"
#include "egg_lib.h"
#include "network.h"
#include "server.h"
#include "channel.h"
#include "user.h"
#include "irc.h"
#include "dcc.h"
#include "trigger.h"
#include "t_crypto_module.h"
#include "t_timer.h"
#include "util.h"

#ifdef HAVE_TCL
#include "tcl_embed.h"
#endif /* HAVE_TCL */

#ifdef HAVE_PHP
#include "php_embed.h"
#endif /* HAVE_PHP */

#ifdef HAVE_JS
#include "js_embed.h"
#endif /* HAVE_JS */

#ifdef HAVE_PYTHON
#include "python_embed.h"
#endif /* HAVE_PYTHON */



/* This is the eggdrop core API that is exported to TCL, PHP, perl, etc */

/* 
 * All commands are derived from that in the Eggdrop TCL lib. They need
 * ported to trollbot's internal storage of data, then wrapped by any 
 * external scripting language in their language specific files.
 *
 * Use the following for a reference on the commented out commands
 * contained here.
 *
 * tcl-commands.doc in the docs/ or docs_dist/
 */


/* 
	 ?  matches any single character
   *  matches 0 or more characters of any type
   %  matches 0 or more non-space characters (can be used to match a single
      word)
   ~  matches 1 or more space characters (can be used for whitespace between
      words)

   returns 1 if no match, 0 if matched

BUG: ~, &, and % don't work
*/
/* This is not an eggdrop function, this should go in troll_lib */
int egg_matchwilds(const char *haystack, const char *needle)
{
	if (needle == NULL || haystack == NULL)
		return 1;

	while (*needle)
	{
		if (*haystack == '\0'){
			/* If *needle is '*', and *(needle+1) = '\0', this should return 0 for success */
			if ((*needle == '*') && *(needle+1) == '\0')
				return 0;
			/* Hit end of haystack but not the ned of needle, so match fails. */
			return 1;
		}


		if (*needle == '?'){
			/* Any character matches, just move on. */
			needle++;
			haystack++;
		}
		else if (*needle == '*'){
			/* Match characters til end of haystack, or until *(needle+1) */
			while (*haystack != '\0' && *haystack != *(needle+1)){
				haystack++;
			}
			needle++;
		}
		else if (*needle == '%'){
			/* FIXME: Wildcard % needs implemented */
			/* WTF, looks like it's implemented to me, what am I missing? */
			while (*haystack != '\0' && !isspace(*haystack) && *haystack != *(needle+1)){
				haystack++;
			}
			needle++;
		}
		else if (*needle == '~'){
			/* FIXME: Wildcard ~ needs implemented */
			if (isspace(*haystack)){
				haystack++;
				while (*haystack != '\0' && isspace(*haystack)){
					haystack++;
				}
				needle++;
			}
			else {
				/* Must match at least one space. */
				return 1;
			}
		}
		else if (*needle != *haystack){
			return 1;
		}
		else {
			/* Two characters match.  Next */
			needle++;
			haystack++;
		}
	}

	if (*haystack == '\0'){
		/* Hit end of haystack and end of needle, so match succeeded */
		return 0;
	}
	else {
		/* Hit end of needled, but not end of haystack, match fails. */
		return 1;
	}
}

/* These functions need queue support */
/* NEED_IMP: None */
/* IMP_IN: TCL, PHP, Python, Javascript */
void egg_putserv(struct network *net, const char *text, int option_next)
{
	/* option_next currently ignored */
	irc_printf(net->sock,text); 
}

/* NEED_IMP: ??? */
/* IMP_IN: ??? */
void egg_puthelp(struct network *net, const char *text, int option_next)
{
	/* option_next currently ignored */
	irc_printf(net->sock,text);
}

/* NEED_IMP: ??? */
/* IMP_IN: ??? */
void egg_putquick(struct network *net, const char *text, int option_next)
{
	/* option_next currently ignored */
	irc_printf(net->sock,text);
}

/* Fully compatible */
/* I highly doubt that */
/* NEED_IMP: ??? */
/* IMP_IN: ??? */
void egg_putkick(struct network *net, const char *chan, const char *nick_list, const char *reason)
{
	irc_printf(net->sock,"KICK %s %s :%s",chan,nick_list,reason);
}

/* Fully compatible */
/* NEED_IMP: Python, PHP */
/* IMP_IN: TCL, Javascript */
void egg_putlog(struct network *net, const char *text)
{
	troll_debug(LOG_DEBUG,text);
}

/* Not compatible */
/* NEED_IMP: TCL, Python, PHP, Javascript */
/* IMP_IN: None */
void egg_putcmdlog(struct network *net, const char *text)
{
	troll_debug(LOG_DEBUG,text);
}

/* Not compatible */
/* NEED_IMP: TCL, Python, PHP, Javascript */
/* IMP_IN: None */
void egg_putxferlog(struct network *net, const char *text)
{
	troll_debug(LOG_DEBUG,text);
}

/* Not compatible */
/* NEED_IMP: TCL, PHP, Javascript, Python */
/* IMP_IN: None */
void egg_putloglev(struct network *net, const char *levels, const char *chan, const char *text)
{
	troll_debug(LOG_DEBUG,text);
}

void egg_dumpfile(struct network *net, const char *nick, const char *filename); 

/*    Not Compatible  Returns: the number of messages in all queues. If a queue is specified,
			only the size of this queue is returned. Valid queues are: mode,
			server, help.
			*/
int egg_queuesize(struct network *net, const char *queue)
{
	return 0;
}

/* Not compatible Returns: the number of deleted lines from the specified queue. */
int egg_clearqueue(struct network *net, const char *queue)
{
	return 0;
}

/* Returns number of users in net, 0 if none */
/* Ready for export */
/* NEED_IMP: Python, Perl */
/* IMP_IN: Javascript, TCL, PHP */
int egg_countusers(struct network *net)
{
	struct user *user;
	int count = 0;  

	if (net->users == NULL)
		return 0;

	user = net->users;

	while (user != NULL)
	{
		count++;
		user = user->next;
	}

	return count;
}

/* Fully compatible */
/* NEED_IMP: Perl, Python  */
/* IMP_IN: Javascript[kicken], PHP, TCL */
int egg_validuser(struct network *net, const char *handle)
{
	struct user *user;

	if ((user = net->users) == NULL)
		return 0;

	while (user != NULL)
	{
		if (!strcmp(handle,user->username))
			return 1;
		user = user->next;
	}

	return 0;
}

/* finduser <nick!user@host> */
/* Eggdrop: Returns: the handle found, or "*" if none */
/* Trollbot: Returns: a user struct, NULL if none */
/* NEED_IMP: Perl, Python */
/* IMP_IN: TCL[poutine], Javascript[kicken], PHP */
struct user *egg_finduser(struct network *net, const char *mask)
{
	struct user *user;

	if ((user = net->users) == NULL)
		return NULL;

	while (user != NULL)
	{
		if (!egg_matchwilds(user->uhost,mask))
			return user;

		user = user->next;
	}

	return NULL;
}

/* net is optional, if NULL, write all user/channel files to disk */
/* NEED_IMP: Perl, Python  */
/* IMP_IN: TCL, Javascript, PHP */
void egg_save(struct network *net)
{
	users_save(net);
	chans_save(net);
}

/* Caller is responsible for memory freeing */
/* NON-EGGDROP COMMAND, CONSIDER MOVING */
char *egg_makepasswd(const char *pass, const char *hash_type)
{
	if ((g_cfg->crypto == NULL) || g_cfg->crypto->create_hash == NULL)
		return NULL;

	return g_cfg->crypto->create_hash(pass, hash_type);
}

/* Fully Compatible */
/* passwdok <handle> <pass> */
/* NEED_IMP: Python */
/* IMP_IN: Javascript[kicken], PHP, TCL */
int egg_passwdok(struct network *net, const char *handle, const char *pass) 
{
	char *hash_string = NULL;
	struct user *user = NULL;

	if ((user = net->users) == NULL)
		return 0;

	while (user != NULL)
	{
		if (!strcmp(handle,user->username))
		{
			/* If pass is null or zero length, or '-', only check to see if user has a password. */
			if (pass == NULL || strlen(pass) == 0 || (strlen(pass)==1 && pass[0] == '-')){
				if (user->passhash != NULL){
					return 1;
				}
				else {
					return 0;
				}
			}

			if (user->hash_type == NULL)
			{
				troll_debug(LOG_ERROR,"Missing Hash Type for user %s\n",user->username);
				return 0;
			}

			if ((hash_string = egg_makepasswd(pass, user->hash_type)) == NULL)
			{
				return 0;
			}

			if (!strcmp(user->passhash,hash_string))
			{
				free(hash_string);
				return 1;
			}
			else
			{
				free(hash_string);
				return 0;
			}
		}

		user = user->next;
	}

	return 0;
}

/* getuser <handle> <entry-type> [extra info] */
/* setuser <handle> <entry-type> [extra info] */

/* All good */
/* NEED_IMP: TCL, Perl, Python */
/* IMP_IN: Javascript[kicken], PHP  */
int egg_chhandle(struct network *net, const char *old, const char *new)
{
	struct user *olduser;
	struct user *users;

	if ((users = net->users) == NULL)
		return 0;

	if (old == NULL || new == NULL)
		return 0;

	olduser = NULL;

	while (users != NULL)
	{
		/* Store the found user for after the loop */
		if (!strcmp(users->username,old))
			olduser = users;

		/* Duplicate username */
		if (!strcmp(users->username,new))
			return 0;

		users = users->next;
	}

	if (olduser == NULL){
		/* User not found */
		return 0;
	}
	free(olduser->username);

	olduser->username = tstrdup(new);

	return 1;
}

#ifdef CLOWNS
/* chattr <handle> [changes [channel]] */
char *egg_chattr(struct network *net, const char *handle, const char *changes, const char *channel)
{
	struct user *user;
	struct channel_flags *cflags;
	char *glob_changes;
	char *chan_changes;
	char *new_chan_flags;
	char *new_glob_flags;
	char *tmp;
	int op=-1; /* opinvalid = -1, op- = 0, op+ = 1 */
	int i;

	/* I know it's cheesy to dynamically allocate a constant, but output is freed by user */
	if (changes == NULL)
		return tstrdup("*");

	/* Find the user */
	for(user=net->users;user != NULL && tstrcasecmp(handle,user->username); user=user->next);

	if (user == NULL)
		return tstrdup("*");

	/* If flags are channel flags */
	if (channel != NULL)
	{
		if (user->chan_flags == NULL)
		{
			/* Make a new one here */
			user->chan_flags       = new_channel_flags(channel,NULL);
			user->chan_flags->prev = NULL;
			user->chan_flags->next = NULL;

			cflags = user->chan_flags;
		}
		else
		{
			/* Find the channel to get the flags */
			for(cflags=user->chan_flags;cflags != NULL && tstrcasecmp(channel,cflags->chan);cflags=cflags->next);

			/* Channel not found, make a new record */
			if (cflags == NULL)
			{
				cflags = user->chan_flags;

				while(cflags->next != NULL)
					cflags = cflags->next;

				cflags->next = new_channel_flags(channel,NULL);
				cflags->next->prev = cflags;
				cflags       = cflags->next;
			}
		}

		/* If | is in there, global flags are in there also */
		if ((tmp = strchr(changes,"|")) != NULL)
		{
			glob_changes = tmalloc0((strlen(changes) - strlen(tmp)) + 1);
			tmp++;
			chan_changes = tstrdup(tmp);

			strncpy(glob_changes,changes,(strlen(changes) - strlen(tmp)));

			new_glob_flags = tmalloc0(strlen(user->flags) + strlen(glob_changes) + 1);

			/* Do changes to global flags */
			for (i=0;glob_changes[i] != '\0';i++)
			{
				switch(glob_changes[i])
				{
					case '+':
						op = 1;
						break;
					case '-':
						op = 0;
						break;
					default:
						if (op == 1)
						{
							/* add to flags */
							tmp = user->flags;

							while (*tmp != '\0')
							{
								if (*tmp == glob_changes[i])
									break;
								else
									tmp++;
							}

							if (*tmp == '\0')
							{
								tmp = new_glob_flags;
								while (*tmp != '\0')
									tmp++;

								*tmp = glob_changes[i];
							}

							break;
						}
						else
						{
							/* Remove from flags */
						}
						break;
				}
			}
		}
		else
		{
			glob_changes = NULL;
			chan_changes = tstrdup(changes);
		}
	}
	else
	{
		glob_changes = tstrdup(changes);
		chan_changes = NULL;
	}

	return NULL;
}    
#endif /* CLOWNS */

/* botattr <handle> [changes [channel]] */
/* Should be part of chattr */

/* matchattr <handle> <flags> [channel] */
/* NEED_IMP: Perl, Python  */
/* IMP_IN: TCL, Javascript, PHP */
int egg_matchattr(struct network *net, const char *handle, const char *flags, const char *channel)
{
	struct user *user;
	int i;


	user = net->users;

	while (user->prev != NULL) user = user->prev;

	while (user != NULL)
	{
		if (!tstrcasecmp(handle,user->username))
			break;

		user = user->next;
	}

	if (user == NULL)
		return 0;

	/* Deal with this either as a channel or global, not sure if this should
	 * handle eggdrop notation a|f or whatever.
	 * FIXME
	 */
	if (channel != NULL)
	{
		troll_debug(LOG_ERROR, "FIXME: egg_matchattr() only works for global flags");
	} 
	else
	{
		if (user->flags == NULL)
			return 0;

		/* No channel, just check global flags */
		for (i=0;flags[i] != '\0';i++)
		{
			if (strchr(user->flags,flags[i]) == NULL)
				return 0;
		}
	}

	return 1;

}

/* adduser <handle> [hostmask] */
/* Need to check if eggdrop saves the userfile after this */
/* NEED_IMP: Python */
/* IMP_IN: Javascript, PHP, TCL */
int egg_adduser(struct network *net, char *username, char *hostmask)
{
	struct channel *chan;
	struct channel_user *cuser;
	struct user *user = NULL;

	if (username == NULL)
		return 0;

	/* Already exists */
	if (network_user_find_by_username(net, username) != NULL)
		return 0;
	

	/* Try to find out as much as you possibly can about the user */
	/* Most likely by going through each channel finding a nick that
	 * matches, and filling in the ident, and so on for the eggdrop
	 * default mask.
	 */
	/* TODO: Test eggdrop behavior to verify it is the same */
	chan = net->chans; /* wtf on naming */
	
	/* go through each channel */
	while (chan != NULL)
	{
		cuser = chan->user_list;

		/* go through each channel user */
		while (cuser != NULL)
		{
			if (!tstrcasecmp(username, cuser->nick))
			{
				break;
			}

			cuser = cuser->next;
		}

		if (cuser != NULL)
			break;

		chan = chan->next;
	}

	/* Add user to users list */

	if (cuser == NULL)
	{
		/* use defaults */
		user = new_user(username, username, NULL, NULL, NULL, hostmask, net->default_flags);
	}
	else
	{
		/* use existing information */
		user = new_user(username, cuser->nick, NULL, cuser->ident, cuser->realname, (hostmask == NULL) ? cuser->host : hostmask, net->default_flags);
	}


	if (user == NULL)
		return 0;

	
	net->users = user_list_add(net->users, user);

	/* TODO: Find out whether to save, going to go ahead and save for now */
	users_save(net);

	return 1;
}

/* addbot <handle> <address> -- This should wrap egg_adduser */

/* deluser <handle> */
/* NEED_IMP: Javascript, TCL, Python, PHP */
/* IMP_IN: None */
/*
  deluser <handle>
    Description: attempts to erase the user record for a handle
    Returns: 1 if successful, 0 if no such user exists
    Module: core
*/
int egg_deluser(struct network *net, char *username)
{
		struct user *user = NULL;

		if (username == NULL)
			return 0;

		/* Doesn't exist exists */
		if ((user = network_user_find_by_username(net, username)) == NULL)
			return 0;

		net->users = user_list_del(net->users, user);

		return 1;
}

/* delhost <handle> <hostmask> */
/* addchanrec <handle> <channel> */
/* delchanrec <handle> <channel> */
/* getchaninfo <handle> <channel> */
/* setchaninfo <handle> <channel> <info> */
/* newchanban <channel> <ban> <creator> <comment> [lifetime] [options] */
/* newban <ban> <creator> <comment> [lifetime] [options] */
/* newchanexempt <channel> <exempt> <creator> <comment> [lifetime] [options] */
/* newexempt <exempt> <creator> <comment> [lifetime] [options] */
/* newchaninvite <channel> <invite> <creator> <comment> [lifetime] [options] */
/* newinvite <invite> <creator> <comment> [lifetime] [options] */


/* stick <banmask> [channel] */
/* NEED_IMP: TCL, Javascript, Python, PHP */
/* IMP_IN: None */
/* Need to verify what this does */
int egg_stick(struct network *net, char *ban, char *channel)
{
	struct channel     *chan;
	struct channel_ban *cban;

	if ((channel == NULL) || strlen(channel) == 0)
	{
		/* TODO: Check Global list, which doesn't exist yet */
		return 0;
	}

	/* Find the channel */
	chan = network_channel_find(net, channel);
	
	if (chan == NULL)
		return 0;

	cban = channel_channel_ban_find(chan, ban);

	if (cban == NULL)
		return 0;

	cban->is_sticky = 1;

	return 1;
}


/* unstick <banmask> [channel] */
/* NEED_IMP: TCL, Javascript, Python, PHP */
/* IMP_IN: None */
/* Need to verify what this does */
int egg_unstick(struct network *net, char *ban, char *channel)
{
	struct channel     *chan;
	struct channel_ban *cban;

	if ((channel == NULL) || strlen(channel) == 0)
	{
		/* TODO: Check Global list, which doesn't exist yet */
		return 0;
	}

	/* Find the channel */
	chan = network_channel_find(net, channel);
	
	if (chan == NULL)
		return 0;

	cban = channel_channel_ban_find(chan, ban);

	if (cban == NULL)
		return 0;

	cban->is_sticky = 0;

	return 1;
}


/* stickexempt <exemptmask> [channel] */
/* unstickexempt <exemptmask> [channel] */
/* stickinvite <invitemask> [channel] */
/* unstickinvite <invitemask> [channel] */
/* killchanban <channel> <ban> */
/* killban <ban> */
/* killchanexempt <channel> <exempt> */
/* killexempt <exempt> */
/* killchaninvite <channel> <invite> */
/* killinvite <invite> */
/* ischanjuped <channel> */

/* isban <ban> [channel] */
/* NEED_IMP: Python */
/* IMP_IN: Javascript, PHP, TCL */
int egg_isban(struct network *net, char *ban, char *channel)
{
	struct channel     *chan;
	struct channel_ban *cban;

	if ((channel == NULL) || strlen(channel) == 0)
	{
		/* TODO: Check Global list, which doesn't exist yet */
		return 0;
	}

	/* Find the channel */
	chan = network_channel_find(net, channel);
	
	if (chan == NULL)
		return 0;

	cban = channel_channel_ban_find(chan, ban);

	if (cban == NULL)
		return 0;

	return 1;
}

/* ispermban <ban> [channel] */
/* NEED_IMP: TCL, Python */
/* IMP_IN: Javascript, PHP */
int egg_ispermban(struct network *net, char *ban, char *channel)
{
	struct channel     *chan;
	struct channel_ban *cban;

	if ((channel == NULL) || strlen(channel) == 0)
	{
		/* TODO: Check Global list, which doesn't exist yet */
		return 0;
	}

	/* Find the channel */
	chan = network_channel_find(net, channel);
	
	if (chan == NULL)
		return 0;

	cban = channel_channel_ban_find(chan, ban);

	if (cban == NULL)
		return 0;
	
	return cban->is_perm;
}

/* isexempt <exempt> [channel] */
/* ispermexempt <exempt> [channel] */
/* isinvite <invite> [channel] */
/* isperminvite <invite> [channel] */

/*
 *   isbansticky <ban> [channel]
 *   Returns: 1 if the specified ban is marked as sticky in the global ban
 *   list; 0 otherwise. If a channel is specified, that channel's ban list
 *   is checked as well.
 *   Module: channels
 */
/* NEED_IMP: TCL, Python */
/* IMP_IN: Javascript, PHP */
int egg_isbansticky(struct network *net,  char *ban, char *channel)
{
	struct channel     *chan;
	struct channel_ban *cban;

	if ((channel == NULL) || strlen(channel) == 0)
	{
		/* TODO: Check Global list, which doesn't exist yet */
		return 0;
	}

	/* Find the channel */
	chan = network_channel_find(net, channel);
	
	if (chan == NULL)
		return 0;

	cban = channel_channel_ban_find(chan, ban);

	if (cban == NULL)
		return 0;
	
	return cban->is_sticky;
}

/* isexemptsticky <exempt> [channel] */
/* isinvitesticky <invite> [channel] */
/* matchban <nick!user@host> [channel] */
/* matchexempt <nick!user@host> [channel] */
/* matchinvite <nick!user@host> [channel] */
/* banlist [channel] */
/* exemptlist [channel] */
/* invitelist [channel] */
/* newignore <hostmask> <creator> <comment> [lifetime] */
/* killignore <hostmask> */
/* ignorelist */
/* isignore <hostmask> */
/* reload */
/* backup */

/* getting-users: Returns 1 if bot is downloading a userfile, 0 if not */
/* NEED_IMP: TCL, PHP, Perl, Python */
/* IMP_IN: Javascript */
int egg_getting_users(struct network *net)
{
	return 0;
}

/* channel add <name> <option-list> */
/* channel set <name> <options...> */
/* channel info <name> */
/* channel get <name> <setting> */
/* channel remove <name> */

/* Eggdrop Compatible */
/* NEED_IMP: Perl */
/* IMP_IN: PHP, Python, TCL, Javascript */
void egg_savechannels(struct network *net)
{
	chans_save(net);
}

/* loadchannels */


/* Eggdrop Compatible */
/* NEED_IMP: PHP, Perl, Python             */
/* IMP_IN: TCL, Javascript[kicken]				 */
/* Note: This appears to have a bug with   
 * Dynamically loaded channels. this is not
 * the issue here, it's in default_triggers.c
 */
/* FIXME: Should this return an array of strings instead of one string? */
char *egg_channels(struct network *net)
{
	int alloc_size      = 0;
	char *ret           = NULL;
	struct channel *tmp = NULL;

	/* Go through and get the size first */
	tmp = net->chans;
	while (tmp->prev != NULL) tmp = tmp->prev;

	while (tmp != NULL)
	{
		if (tmp->name != NULL)
			alloc_size += strlen(tmp->name) + 1;

		tmp = tmp->next;
	}

	/* Now make a string */
	ret = tmalloc0(alloc_size + 1);
	tmp = net->chans;

	while (tmp->prev != NULL) tmp = tmp->prev;

	while (tmp != NULL)
	{
		if (tmp->name != NULL)
		{
			if (tmp->next == NULL)
				sprintf(&ret[strlen(ret)],"%s",tmp->name);
			else
				sprintf(&ret[strlen(ret)],"%s ",tmp->name);
		}

		tmp = tmp->next;
	}

	return ret;
}

/* isbotnick: Returns 1 if nick is bot's nick */
/* NEED_IMP: PHP, Perl, Python  */
/* IMP_IN: Javascript */
int egg_isbotnick(struct network *net, char *nick)
{
	return (!tstrcasecmp(net->botnick,nick));
}

/* botisop [channel] */
/* NEED_IMP: TCL, PHP, Python */
/* IMP_IN: Javascript */
int egg_botisop(struct network *net, const char *channel)
{
	return egg_isop(net, egg_botnick(net), channel);
}

/* botishalfop [channel] */

/* botisvoice [channel] */
/* NEED_IMP: Javascript, TCL, PHP, Python */
int egg_botisvoice(struct network *net, const char *nickname, const char *channel)
{
	return egg_isvoice(net, egg_botnick(net), channel);
}


/* botonchan [channel] */
/* NEED_IMP: Javascript, TCL, PHP, Python */
int egg_botonchan(struct network *net, const char *nickname, const char *channel)
{
	/* This function is stupid, but part of eggdrop */
	return egg_onchan(net, egg_botnick(net), channel);
}

/* isop <nickname> [channel]
 *   isop <nickname> [channel]
 *     Returns: 1 if someone by the specified nickname is on the channel (or
 *     any channel if no channel name is specified) and has ops; 0 otherwise
 *     Module: irc
 */
/* NEED_IMP: PHP, Python */
/* IMP_IN: Javascript, TCL */
int egg_isop(struct network *net, const char *nickname, const char *channel)
{
	struct channel      *chan  = NULL;
	struct channel_user *cuser = NULL;

	if ((channel == NULL) || strlen(channel) == 0)
	{
		/* check all channels */
		chan = net->chans;

		/* No channels are on bot */
		if (chan == NULL)
			return 0;

		while (chan != NULL)
		{
			cuser = channel_channel_user_find(chan, nickname);
		
			if (cuser != NULL)
				if (cuser->modes != NULL)
					if (strchr(cuser->modes, 'o') != NULL)
						return 1;

			chan = chan->next;
		}
	
		return 0;
	}

	chan = network_channel_find(net, channel);

	if (chan == NULL)
		return 0;

	cuser = channel_channel_user_find(chan, nickname);

	if (cuser == NULL)
		return 0;

	if (cuser->modes == NULL)
		return 0;

	if (strchr(cuser->modes, 'o') == NULL)
		return 0;

	return 1;
}


/* ishalfop <nickname> [channel] */
/* wasop <nickname> <channel> */
/* washalfop <nickname> <channel> */

/* isvoice <nickname> [channel]
 *   Returns: 1 if someone by that nickname is on the channel (or any
 *   channel if no channel is specified) and has voice (+v); 0 otherwise
 *    Module: irc
 */
/* NEED_IMP: PHP, Python */
/* IMP_IN: TCL, Javascript */
int egg_isvoice(struct network *net, const char *nickname, const char *channel)
{
	struct channel      *chan  = NULL;
	struct channel_user *cuser = NULL;

	if ((channel == NULL) || strlen(channel) == 0)
	{
		/* check all channels */
		chan = net->chans;

		/* No channels are on bot */
		if (chan == NULL)
			return 0;

		while (chan != NULL)
		{
			cuser = channel_channel_user_find(chan, nickname);
		
			if (cuser != NULL)
				if (cuser->modes != NULL)
					if (strchr(cuser->modes, 'v') != NULL)
						return 1;

			chan = chan->next;
		}
	
		return 0;
	}

	chan = network_channel_find(net, channel);

	if (chan == NULL)
		return 0;

	cuser = channel_channel_user_find(chan, nickname);

	if (cuser == NULL)
		return 0;

	if (cuser->modes == NULL)
		return 0;

	if (strchr(cuser->modes, 'v') == NULL)
		return 0;

	return 1;
}

/** 
 * Eggdrop Compatible onchan
 * @param net A network struct where this is to be checked.
 * @param nickname The nickname to try and find.
 * @param channel Optional channel name to check in
 * @return 1 if user found on optional channel, 1 if user found without optional channel, 0 if user not found on optional channel or 0 if not found without optional channel
 */
/* NEED_IMP: Python, Perl, PHP */
/* IMP_IN: TCL, Javascript */
int egg_onchan(struct network *net, char *nickname, char *channel)
{
	struct channel      *chan  = NULL;
	struct channel_user *cuser = NULL;

	if (net == NULL)
	{
		troll_debug(LOG_ERROR,"egg_onchan called with NULL network struct");
		return 0;
	}

	if (net->chans == NULL)
	{
		troll_debug(LOG_WARN,"egg_onchan called with NULL chans inside the net struct");
		return 0;
	}

	chan = net->chans;

	/* should use new list stuff in util */
	while (chan->prev != NULL) chan = chan->prev;

	if ((channel != NULL) && strlen(channel) > 0)
	{
		while (chan != NULL)
		{
			if (!tstrcasecmp(channel,chan->name))
			{
				if (chan->user_list == NULL)
					return 0; /* This would probably be a bug if occurred */

				cuser = channel_channel_user_find(chan, nickname);

				if (cuser == NULL)
					return 0;
		
				return 1;
			}

			chan = chan->next;
		}

		if (chan == NULL)
		{
			/* Nick is not on chan */
			return 0;
		}
	}
	else
	{
		while (chan != NULL)
		{
			if (chan->user_list != NULL)
			{

				cuser = channel_channel_user_find(chan, nickname);

				if (cuser != NULL)
					return 1;

			}

			chan = chan->next;
		}

		return 0; /* User not found */
	}

	return 0; /* never should be reached */
}

/* Handle in trollbot is the nick */
/* nick2hand <nickname> [channel] */
/* hand2nick <handle> [channel] */

/* Could link to egg_onchan() */
/* handonchan <handle> [channel] */



/* ischanban <ban> <channel>
 * Returns: 1 if the specified ban is on the given channel's ban list
 * (not the bot's banlist for the channel)
 * Module: irc
 */
/* NEED_IMP: Javascript, PHP, TCL, Python */
int egg_ischanban(struct network *net, const char *ban, const char *channel)
{
	struct channel      *chan  = NULL;
	struct channel_ban  *cban  = NULL;

	chan = network_channel_find(net, channel);

	if (chan == NULL)
		return 0;

	cban = channel_channel_ban_find(chan, ban);

	if (cban == NULL)
		return 0;

	return 1;
}

/* ischanexempt <exempt> <channel> */
/* ischaninvite <invite> <channel> */

/* chanbans <channel> */
/*  chanbans <channel>
 *  Returns: a list of the current bans on the channel. Each element is
 *    a sublist of the form {<ban> <bywho> <age>}. age is seconds from the
 *    bot's point of view
 *  Module: irc
 *  bywho: uhost of setter.
 *  age: needs an eggdrop semantic check
 *  Age starts over on eggdrop if it for any reason leaves the channel. Quite
 *  an odd thing to do as that information would be available to it. Perhaps
 *  this is a candidate for eggdrop strict versus trollbot modes.
 */
/* NEED_IMP: Javascript, PHP, Python, TCL */
/* IMP_IN: None */
#ifdef CLOWNS
char **egg_chanbans(struct network *net, const char *channel)
{
	struct channel *chan;
	struct channel_ban *cban;
	char **ret;
	int count = 0;

	/* If channel not found return NULL */
	if ((chan = network_channel_find(net, channel)) == NULL)
		return NULL;

	/* Scan through the bans to get a count */
	cban = chan->banlist;

	while (cban != NULL)
	{
		count++;
		cban = cban->next;
	}

	ret = tmalloc0(sizeof(char *) * count + 1);

	cban = cban->banlist;
	
	for (count);
	while (chan != NULL)
	{
		
	}

}
#endif /* CLOWNS */

/* chanexempts <channel> */
/* chaninvites <channel> */
/* resetbans <channel> */
/* resetexempts <channel> */
/* resetinvites <channel> */
/* resetchan <channel> */
/* getchanhost <nickname> [channel] */
/* getchanjoin <nickname> <channel> */
/* onchansplit <nick> [channel] */
/* chanlist <channel> [flags[&chanflags]] */
/* getchanidle <nickname> <channel> */
/* getchanmode <channel>
 *   getchanmode <channel>
 *     Returns: string of the type "+ntik key" for the channel specified
 *     Module: irc
 */
/* NEED_IMP: PHP, Python */
/* IMP_IN: Javascript, TCL */
char *egg_getchanmode(struct network *net, const char *channel)
{
	struct channel *chan = NULL;

	chan = network_channel_find(net, channel);

	/* TODO: Error handling */
	if (chan == NULL)
		return NULL;	

	return tstrdup(chan->chanmode);
}

/* jump [server [port [password]]] */
/* pushmode <channel> <mode> [arg] */
/* flushmode <channel> */

/*
 *   topic <channel>
 *       Returns: string containing the current topic of the specified channel
 *       Module: irc
 */
/* NEED_IMP: PHP, Python, Perl */
/* IMP_IN: TCL, Javascript */
/* Eggdrop Compatible */
char *egg_topic(struct network *net, char *chan)
{
	struct channel *channel = NULL;

	/* Should be guaranteed by the *_lib func, but return a sane
 	 * value should this ever be encountered.
 	 */
	if (net == NULL || ((channel = network_channel_find(net,chan)) == NULL))
		return tstrdup(""); /* tstrdup since caller frees */

	return tstrdup(channel->topic);
}

/* validchan <channel> PROBLEM: trollbot doesn't track chans like required for this */
/* isdynamic <channel> */

/* NEED_IMP: PHP, Perl, Python */
/* IMP_IN: Javascript, TCL */
void egg_putdcc(struct network *net, int idx, const char *text)
{
	struct dcc_session *dtmp;

	dtmp = net->dccs;

	while (dtmp != NULL)
	{
		if (dtmp->id == idx)
		{
			irc_printf(dtmp->sock,text);
			return;
		}

		dtmp = dtmp->next;
	}

	return; 
}

/* dccbroadcast <message> */
/* Trollbot: Only broadcasts on the net specified */
/* NEED_IMP: TCL, PHP, Perl, Python */
/* IMP_IN: Javascript */
void egg_dccbroadcast(struct network *net, const char *message)
{
	struct dcc_session *dtmp;

	dtmp = net->dccs;

	while (dtmp != NULL)
	{
		if (dtmp->status >= DCC_NOTREADY)
		{
			irc_printf(dtmp->sock,message);
		}

		dtmp = dtmp->next;
	}

	return;
}

/* dccputchan <channel> <message> */
/* boot <user@bot> [reason] */
/* dccsimul <idx> <text> */

/* Returns 0 on error, an idx if successful */
/* NEED_IMP: TCL, PHP, Perl, Python, Javascript */
int egg_hand2idx(struct network *net, const char *handle)
{
	struct dcc_session *dtmp;

	dtmp = net->dccs;

	while (dtmp != NULL)
	{
		if (dtmp->status >= DCC_NOTREADY)
		{
			if (dtmp->user != NULL)
			{
				if (!strcmp(dtmp->user->username,handle))
					return dtmp->id;
			}
		}

		dtmp = dtmp->next;
	}

	return 0;
}



/* returns a user struct or NULL based on whether it's found */
/* NEED_IMP: TCL, PHP, Perl, Python, Javascript */
struct user *egg_idx2hand(struct network *net, int idx)
{
	struct dcc_session *dtmp;

	dtmp = net->dccs;

	while (dtmp != NULL)
	{
		if (dtmp->status >= DCC_NOTREADY)
			return dtmp->user;

		dtmp = dtmp->next;
	}

	return NULL;
} 

/* valididx <idx>
 *	 Returns: 1 if the idx currently exists; 0 otherwise
 * Module: core
 */
/* NEED_IMP: TCL, PHP, Perl, Python, Javascript */
int egg_valididx(struct network *net, int idx)
{
	struct dcc_session *dtmp;
	
	dtmp = net->dccs;

	while (dtmp != NULL)
	{
		if (dtmp->id == idx && dtmp->status >= DCC_NOTREADY)
			return 1;
	
		dtmp = dtmp->next;
	}

	return 0;
}


/* getchan <idx> */
/* setchan <idx> <channel> */
/* console <idx> [channel] [console-modes] */
/* echo <idx> [status] */
/* strip <idx> [+/-strip-flags] */
/* putbot <bot-nick> <message> */
/* putallbots <message> */
/* killdcc <idx> */
/* bots */
/* botlist */
/* islinked <bot> */
/* dccused */
/* dcclist ?type? */
/* whom <chan> */
/* getdccidle <idx> */
/* getdccaway <idx> */
/* setdccaway <idx> <message> */
/* connect <host> <port> */
/* listen <port> <type> [options] [flag] */
/* dccdumpfile <idx> <filename> */
/* notes <user> [numberlist] */
/* erasenotes <user> <numberlist> */
/* listnotes <user> <numberlist> */
/* storenote <from> <to> <msg> <idx> */
/* assoc <chan> [name] */
/* killassoc <chan> */
/* compressfile [-level <level>] <src-file> [target-file] */
/* uncompressfile <src-file> [target-file] */
/* iscompressed <filename> */
/* setpwd <idx> <dir> */
/* getpwd <idx> */
/* getfiles <dir> */
/* getdirs <dir> */
/* dccsend <filename> <ircnick> */
/* filesend <idx> <filename> [ircnick] */
/* setdesc <dir> <file> <desc> */
/* getdesc <dir> <file> */
/* setowner <dir> <file> <handle> */
/* getowner <dir> <file> */
/* setlink <dir> <file> <link> */
/* getlink <dir> <file> */
/* getfileq <handle> */
/* getfilesendtime <idx> */
/* mkdir <directory> [<required-flags> [channel]] */
/* rmdir <directory> */
/* mv <file> <destination> */
/* cp <file> <destination> */
/* getflags <dir> */
/* setflags <dir> [<flags> [channel]] */

/*
 * Description: 
 * 'bind' is used to attach procedures to certain events. 
 * flags are the flags the user must have to trigger the event (if applicable). 
 * proc-name is a pointer to the procedure to call for this command.
 * If the proc-name is NULL, no binding is added. 
 * Returns: 
 * name of the command that was added, or (if proc-name is NULL), a list of the current bindings for this command
*/
/* IMP_IN: Javascript, TCL */
char **egg_bind(struct network *net, char *type, char *flags, char *mask, char *cmd, void (*handler)(struct network *, struct trigger *, struct irc_data *, struct dcc_session *, const char *))
{
	char **returnValue=NULL;
	if (handler == NULL){
		struct trigger *trigger = NULL;
		struct trigger *triggerListHead = NULL;
		int numMatches=0;

		/* Get a list of binds matching mask and return that list. */
		if (!strcmp("pub",type)){ triggerListHead = net->trigs->pub;  }
		else if (!strcmp("pubm",type)){ triggerListHead = net->trigs->pubm; }
		else if (!strcmp("msg",type)){  triggerListHead = net->trigs->msg; }
		else if (!strcmp("msgm",type)){  triggerListHead = net->trigs->msgm; }
		else if (!strcmp("notc",type)){  triggerListHead = net->trigs->notc; }
		else if (!strcmp("join",type)){  triggerListHead = net->trigs->join; }
		else if (!strcmp("part",type)){  triggerListHead = net->trigs->part; }
		else if (!strcmp("dcc",type)){  triggerListHead = net->trigs->dcc; }
		else if (!strcmp("raw",type)){  triggerListHead = net->trigs->raw; }
		else if (!strcmp("ctcp",type)){  triggerListHead = net->trigs->ctcp; }
		else if (!strcmp("topc",type)){	triggerListHead = net->trigs->topc; }

		trigger = triggerListHead;
		while (trigger != NULL){
			if (!strcmp(trigger->mask, mask)){
				numMatches=0;
			}
			trigger = trigger->next;
		}

		returnValue = tmalloc0(sizeof(*returnValue)*(numMatches+1));

		numMatches=0;
		while (trigger != NULL){
			if (!strcmp(trigger->mask, mask)){
				returnValue[numMatches++]=tstrdup(trigger->command);
			}
			trigger = trigger->next;
		}
	}
	else {
		returnValue = tmalloc0(sizeof(*returnValue));
		/* Needs to check stackable, and whether dupes exist FIXME */
		if (!strcmp("pub",type))
		{
			trigger_list_add(&net->trigs->pub,new_trigger(NULL,TRIG_PUB,mask,cmd,handler));
			*returnValue=tstrdup(cmd);
		}
		else if (!strcmp("pubm",type))
		{
			trigger_list_add(&net->trigs->pubm,new_trigger(NULL,TRIG_PUBM,mask,cmd,handler));
			*returnValue=tstrdup(cmd);
		}
		else if (!strcmp("msg",type))
		{
			trigger_list_add(&net->trigs->msg,new_trigger(NULL,TRIG_MSG,mask,cmd,handler));
			*returnValue=tstrdup(cmd);
		}
		else if (!strcmp("msgm",type))
		{
			trigger_list_add(&net->trigs->msgm,new_trigger(NULL,TRIG_MSGM,mask,cmd,handler));
			*returnValue=tstrdup(cmd);
		}
		else if (!strcmp("notc",type))
		{
			trigger_list_add(&net->trigs->notc,new_trigger(NULL,TRIG_NOTC,mask,cmd,handler));
			*returnValue=tstrdup(cmd);
		}  
		else if (!strcmp("join",type))
		{
			trigger_list_add(&net->trigs->join,new_trigger(NULL,TRIG_JOIN,mask,cmd,handler));
			*returnValue=tstrdup(cmd);
		}
		else if (!strcmp("part",type))
		{ 
			trigger_list_add(&net->trigs->part,new_trigger(NULL,TRIG_PART,mask,cmd,handler));
			*returnValue=tstrdup(cmd);
		}
		else if (!strcmp("sign",type))
		{ 
			trigger_list_add(&net->trigs->sign,new_trigger(NULL,TRIG_SIGN,mask,cmd,handler));
			*returnValue=tstrdup(cmd);
		}
		else if (!strcmp("ctcp",type))
		{ 
			trigger_list_add(&net->trigs->ctcp,new_trigger(NULL,TRIG_CTCP,mask,cmd,handler));
			*returnValue=tstrdup(cmd);
		}
		else if (!strcmp("dcc",type))
		{
			trigger_list_add(&net->trigs->dcc,new_trigger(NULL,TRIG_DCC,mask,cmd,handler));
			*returnValue=tstrdup(cmd);
		}
		else if (!strcmp("topc",type))
		{
			trigger_list_add(&net->trigs->topc,new_trigger(NULL,TRIG_TOPC,mask,cmd,handler));
			*returnValue=tstrdup(cmd);
		}
		else if (!strcmp("raw",type))
		{
			trigger_list_add(&net->trigs->raw,new_trigger(NULL,TRIG_RAW,mask,cmd,handler));    
			*returnValue=tstrdup(cmd);
		}
	}
	return returnValue;
}

/* unbind <type> <flags> <keyword/mask> <proc-name> */
/*
 *   unbind <type> <flags> <keyword/mask> <proc-name>
 *    Description: removes a previously created bind
 *    Returns: name of the command that was removed
 *   Module: core
 */               
/* Returns NULL if no binding exists, eggdrop error message: no such binding */
/* Tested on Eggdrop for behavior with undefined events, the following occurs:
 *   If flags does not match, it is removed anyways
 *   What must match is mask and command
 *   mask is case sensitive, command suprisingly is not
 */
/* NEED_IMP: NONE */
/* IMP_IN: Javascript, PHP, Python, TCL*/
char *egg_unbind(struct network *net, char *type, char *flags, char *mask, char *command)
{
	struct trigger *trig_list = NULL;
	struct trigger *trig_head = NULL;
	char *ret               = NULL;

	/* Case insensitive, checked eggdrop */
	if (!tstrcasecmp("pub",type))
		trig_head = trig_list = net->trigs->pub;
	else if (!tstrcasecmp("pubm",type))
		trig_head = trig_list = net->trigs->pubm;
	else if (!tstrcasecmp("msg",type))
		trig_head = trig_list = net->trigs->msg;
	else if (!tstrcasecmp("msgm",type))
		trig_head = trig_list = net->trigs->msgm;
	else if (!tstrcasecmp("notc",type))
		trig_head = trig_list = net->trigs->notc;
	else if (!tstrcasecmp("join",type))
		trig_head = trig_list = net->trigs->join;
	else if (!tstrcasecmp("part",type))
		trig_head = trig_list = net->trigs->part;
	else if (!tstrcasecmp("dcc",type))
		trig_head = trig_list = net->trigs->dcc;
	else if (!tstrcasecmp("topc",type))
		trig_head = trig_list = net->trigs->topc;
	else if (!tstrcasecmp("raw",type))
		trig_head = trig_list = net->trigs->raw;
	else
	{
		troll_debug(LOG_DEBUG, "egg_unbind called for trigger type of (%s), not recognized or handled",type);
		return NULL;
	}

	while (trig_list != NULL)
	{
		/* Verified case sensitive on eggdrop */
		if (!strcmp(mask, trig_list->mask) && !tstrcasecmp(command, trig_list->command))
		{
			trigger_list_del(trig_head, trig_list);
			/* Saves reallocation */
			ret = trig_list->mask;
			trig_list->mask = NULL;
			free_trigger(trig_list);
			return ret;
		}

		trig_list = trig_list->next;
	}

	return NULL;
}



/* binds ?type/mask? */
/* logfile [<modes> <channel> <filename>] */
/* maskhost <nick!user@host> */

/* timer <minutes> <tcl-command> */
/* NEED_IMP: TCL, PHP, Perl, Python, Javascript */
/* DO NOT TOUCH THIS */
int egg_utimer(struct network *net, int seconds, char *command, void (*handler)(struct network *, struct t_timer *))
{
  struct t_timer *timer = NULL;

  timer = t_timer_new();

  timer->time_set  = time(NULL);
  timer->time_trig = (time(NULL) + seconds);
  timer->command   = tstrdup(command);
  timer->handler   = handler;
	timer->net 	     = net;

  net->timers = t_timer_add(net->timers,timer);

  /* FIXME */
  return -1;
}

/* timer <minutes> <tcl-command> */
/* NEED_IMP: TCL, PHP, Perl, Python, Javascript */
/* DO NOT TOUCH THIS */
int egg_timer(struct network *net, int minutes, char *command, void (*handler)(struct network *, struct t_timer *))
{
	struct t_timer *timer = NULL;
	
	timer = t_timer_new();

	timer->time_set  = time(NULL);
	timer->time_trig = timer->time_set + (60*minutes);
	timer->command   = tstrdup(command);
	timer->handler   = handler;

	net->timers = t_timer_add(net->timers,timer);

	/* FIXME */
	return -1;
}

/* timers */
/* utimers */
/* killtimer <timerID> */
/* killutimer <timerID> */

/* Consider not implementing in languages which already have this BS */
/* NEED_IMP: TCL, PHP, Perl, Python, Javascript */
time_t egg_unixtime(void)
{
	return time(NULL);
}

/* duration <seconds> */
/* strftime <formatstring> [time] */
/* ctime <unixtime> */
/* myip */
/* control <idx> <command> */
/* sendnote <from> <to[@bot]> <message> */
/* link [via-bot] <bot> */
/* unlink <bot> */
/* encrypt <key> <string> */
/* decrypt <key> <encrypted-base64-string> */

/* encpass <password> */
/* This will have issues due to the password style switchover needs documented */
/* In Trollbot: Encrypts based on hash_type in g_cfg */
/* In Eggdrop: uses blowfish or somesuch */
/* NEED_IMP: PHP, Perl, Python, Javascript */
char *egg_encpass(const char *pass)
{
	return egg_makepasswd(pass, g_cfg->hash_type);
}


/* die [reason] */
/* Require network for log message */
/* NEED_IMP: PHP, Python */
/* IMP_IN: TCL, Javascript */
void egg_die(struct network *net, const char *reason)
{
	struct network *tmp;

	tmp = g_cfg->networks;

	while (tmp != NULL)
	{
		if (reason == NULL)
			irc_printf(tmp, "QUIT");
		else
			irc_printf(tmp, "QUIT :%s",reason);

		/* Flush out the socket TODO: Make function for this */
		shutdown(net->sock, SHUT_RDWR);
		net->sock = -1;
		
		tmp = tmp->next;
	}

	die_nicely(0);
}

/* unames */
/* dnslookup <ip-address/hostname> <proc> [[arg1] [arg2] ... [argN]] */
/* md5 <string> */
/* callevent <event> */
/* modules */
/* loadmodule <module> */
/* unloadmodule <module> */
/* loadhelp <helpfile-name> */
/* unloadhelp <helpfile-name> */
/* reloadhelp */
/* restart */

/* NEED_IMP: TCL, PHP, Perl, Python, Javascript */
void egg_rehash(void)
{
	struct tconfig_block *tcfg;
	struct config *oldcfg;
	struct config *newcfg;
	struct network *otmp;
	struct network *ntmp;

	oldcfg = g_cfg;

	/* FIXME: FFS CONSTANTS?!?!?! */
	tcfg  = file_to_tconfig("trollbot.conf");

	newcfg = config_engine_load(tcfg);

	/* keep a copy in the global config */
	newcfg->tcfg = tcfg;

	/* We need to match the old networks to new ones
	 * in case they were renamed, deleted, or added
	 * to properly disconnect or connect  
	 */
	/* Copy over the TCL and JS interpreter shit */

	/* TODO: Get rid of this shit */
	otmp = oldcfg->networks;
	while (otmp->prev != NULL) otmp = otmp->prev;

	ntmp = newcfg->networks;

	/* Figure out what to free from the old, add chans, etc to new */
	while (otmp != NULL)
	{
		while (ntmp->prev != NULL) ntmp = ntmp->prev;

		while (ntmp != NULL)
		{
			if (!tstrcasecmp(otmp->label,ntmp->label))
			{
				/* Found a match copy over stuff */

				ntmp->dccs = otmp->dccs;
				otmp->dccs = NULL;

				ntmp->dcc_listener = otmp->dcc_listener;

				free_servers(ntmp->servers);
				ntmp->servers = otmp->servers;
				otmp->servers = NULL;

				/* Need to match this by string 
					 ntmp->cur_server = otmp->cur_server;
					 otmp->cur_server = NULL; */

				free_channels(ntmp->chans);
				ntmp->chans = otmp->chans;
				otmp->chans = NULL;

				ntmp->sock  = otmp->sock;
				otmp->sock  = -1;

				if (ntmp->shost != NULL) free(ntmp->shost);
				ntmp->shost = otmp->shost;
				otmp->shost = NULL;				

				ntmp->status = otmp->status;
				/* Don't know what to set for the old */

				free_users(ntmp->users);
				ntmp->users = otmp->users;
				otmp->users = NULL;



				printf("Copied over trigs\n");
				ntmp->trigs = otmp->trigs;
				/* Bug, cannot NULL out trigs here, they aren't being freed either FIXME */	

#ifdef HAVE_TCL
				ntmp->tclinterp = otmp->tclinterp;
				otmp->tclinterp = NULL;
#endif /* HAVE_TCL */

#ifdef HAVE_PERL
				ntmp->perlinterp = otmp->perlinterp;
				otmp->perlinterp = NULL;
#endif /* HAVE_PERL */

#ifdef HAVE_PYTHON
				ntmp->pydict = otmp->pydict;
				otmp->pydict = NULL;
#endif /* HAVE_PYTHON */

#ifdef HAVE_JS
				ntmp->cx = otmp->cx;
				otmp->cx = NULL;
				ntmp->global = otmp->global;
				otmp->global = NULL;
				ntmp->plain_cx = otmp->plain_cx;
				otmp->plain_cx = NULL;
				ntmp->plain_global = otmp->plain_global;
				otmp->plain_global = NULL;

				/* So functions can access it */
				JS_SetContextPrivate(ntmp->cx, ntmp);
#endif /* HAVE_JS */
			}

			ntmp = ntmp->next;
		}

		otmp = otmp->next;
		ntmp = newcfg->networks;
		free(otmp);
	}

	g_cfg = newcfg;

#ifdef HAVE_TCL
	/* For now just reload TCL and js scripts */
	tcl_load_scripts_from_config(g_cfg);
#endif /* HAVE_TCL */

#ifdef HAVE_JS	
	newcfg->js_rt = oldcfg->js_rt;
	oldcfg->js_rt = NULL;
	js_load_scripts_from_config(g_cfg);
#endif /* HAVE_JS */


	free(oldcfg);
}

// Not egg function, egg variable, do not free
char *egg_botnick(struct network *net)
{
	return net->botnick;
}


/* New, add entry to all langs [Done: TCL] */
/* Needs return freed */
char *egg_botname(struct network *net)
{
	char *ret;

	/* + 2 is for the ! and @ */
	ret = tmalloc0(strlen(net->nick) + strlen(net->ident) + strlen(net->shost) + 2 + 1);

	sprintf(ret,"%s!%s@%s",net->nick,net->ident,net->shost);

	return ret;
}

/* New, check functionality, add entry to all langs */
char *egg_version(void)
{
	return VERSION;
}

/* numversion */
/* uptime */
/* server-online */
/* lastbind */
/* isjuped */

int handlen(struct network *net)
{
	return net->handlen;
}

/* config */

