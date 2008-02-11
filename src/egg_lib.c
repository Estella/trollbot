#include "main.h"
#include "egg_lib.h"
#include "network.h"
#include "server.h"
#include "channel.h"
#include "user.h"
#include "irc.h"
#include "dcc.h"
#include "trigger.h"

/* This is the eggdrop core API that is exported to TCL, PHP, perl, etc */


/* This could be replaced by a ltrim() */
char *egg_makearg(char *rest, char *mask)
{
  char *ret;

  ret = &rest[strlen(mask)];

  while (*ret != '\0' && (*ret == ' ' || *ret == '\t'))
    ret++;

  if (ret == NULL)
    return "";

  return ret; 
}

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
int egg_matchwilds(const char *haystack, const char *needle)
{
  if (needle == NULL || haystack == NULL)
    return 1;

  while (*needle)
  {
    /* ? = one single character *haystack is not NULL if *needle isn't */
    if (*needle == *haystack || *needle == '?')
    {
      needle++;
      haystack++;
    } 
    else if (*haystack == '\0' && *needle != '*')
      return 1;
    else if (*needle != '%' && *needle != '~' && *needle != '*')
      return 1;
    else if (*needle == '~')
    {
      needle++;

      /* One or more space characters */
      if (*haystack != ' ' && *haystack != '\t')
        return 1;

      while (*haystack && *needle)
      {
        if (*haystack == *needle)
        {
          /* Greedy lookahead */
          if (!egg_matchwilds(haystack+1,needle-1))
            return 0;

          break;
        }

        if (*haystack != ' ' && *haystack != '\t')
          break;
         
        haystack++;
      }

    }
    /* 0 or more non-space */
    else if (*needle == '%')
    {
      needle++;

      while (*haystack && *needle)
      {
        if (*haystack == *needle)
        {
          /* Greedy lookahead */
          if (!egg_matchwilds(haystack+1,needle-1))
            return 0;
          
          break;
        }

        if (*haystack == ' ' || *haystack == '\t')
          break;

        haystack++;
      }
    }
    else if (*needle == '*')
    {
      needle++;

      while (*haystack && *needle)
      {
        if (*haystack == *needle)
        {
          /* Greedy, but works okay */
          if (!egg_matchwilds(haystack+1,needle-1))
            return 0;
         
          break;
        }

        haystack++;
      }
    }
  }

  return 0;
}

/* These functions need queue support */
void egg_putserv(struct network *net, const char *text, int option_next)
{
  /* option_next currently ignored */
  irc_printf(net->sock,text); 
}

void egg_puthelp(struct network *net, const char *text, int option_next)
{
  /* option_next currently ignored */
  irc_printf(net->sock,text);
}

void egg_putquick(struct network *net, const char *text, int option_next)
{
  /* option_next currently ignored */
  irc_printf(net->sock,text);
}

/* Fully compatible */
void egg_putkick(struct network *net, const char *chan, const char *nick_list, const char *reason)
{
  irc_printf(net->sock,"KICK %s %s :%s",chan,nick_list,reason);
}

/* Fully compatible */
void egg_putlog(struct network *net, const char *text)
{
  troll_debug(LOG_DEBUG,text);
}

/* Not compatible */
void egg_putcmdlog(struct network *net, const char *text)
{
  troll_debug(LOG_DEBUG,text);
}

/* Not compatible */
void egg_putxferlog(struct network *net, const char *text)
{
  troll_debug(LOG_DEBUG,text);
}

/* Not compatible */
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




/* Needs returns checked */
/* Also needs to support different encryption types */
/* userlist [flags] */
int egg_passwdok(struct network *net, const char *handle, const char *pass) 
{
  SHA1_CTX context;
  char digest[20];

  struct user *user;

  if ((user = net->users) == NULL)
    return 0;

  while (user != NULL)
  {
    if (!strcmp(handle,user->username))
    {
      /* NEEDS PASS CHECKED */
      SHA1Init(&context);
      SHA1Update(&context, (unsigned char *)pass, strlen(pass));
      SHA1Final((unsigned char*)digest, &context);
 
      /* Pass is all good baby */
      if (user->passhash != NULL)
      {
        /* Passhash should be unsigned */
        if (!strcmp(user->passhash,digest))
          return 1;
      }

      return 0;
    }

    user = user->next;
  }

  return 0;
}

/* getuser <handle> <entry-type> [extra info] */
/* setuser <handle> <entry-type> [extra info] */

/* All good */
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

  /* I know it's cheesy to dynamically allocate a constant, but output is freed by user */
  if (changes == NULL)
    return tstrdup("*");

  /* Find the user */
  for(user=net->users;user != NULL && strcmp(handle,user->username);user=user->next);

  if (user == NULL)
    return tstrdup("*");

  if (channel != NULL)
  {
    if (user->chan_flags == NULL)
    {
      /* Make a new one here */
      user->chan_flags = new_channel_flags(channel,NULL);
      user->chan_flags->prev = NULL;
      user->chan_flags->next = NULL;
    }
    else
    {
      /* Find the channel to get the flags */
      for(cflags=user->chan_flags;cflags != NULL && strcmp(channel,cflags->chan);cflags=cflags->next);
         
      /* Channel not found, make a new record */
      if (cflags == NULL)
      {
        cflags = user->chan_flags;

        while(cflags->next != NULL)
          cflags = cflags->next;

        cflags->next = new_channel_flags(channel,NULL);
        cflags->next->prev = cflags;
        cflags->next->next = NULL;
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
      for (i=0;*(glob_changes+i) != NULL;i++)
      {
        switch(*(glob_changes+i));
        {
          case '+':
            op = 1;
            break;
          case '-':
            op = 0;
            break;
          default:
            switch (op)
            {
              case -1:
                break;
              case  1: /* add to flags */
                tmp = user->flags;

                while (*tmp != '\0')
                  if (*tmp == *(glob_changes+i))
                    break;
                  else
                    tmp++;

                if (*tmp == '\0')
                {
                  tmp = new_glob_flags;
                  while (*tmp != '\0')
                    tmp++;

                  *tmp = *(glob_changes+i);
                }
                    
                break;
              case  1:
                break;
            }
        }
    }
    else
    {
      glob_flags = NULL;
      chan_flags = tstrdup(changes);
    }

    /* The new chan flags can only be as large as 
     * strlen(oldflags) + glob_flags|chan_flags
     * I'll use that instead of being more accurate
     */
  }
}    
#endif /* CLOWNS */    
      

/* botattr <handle> [changes [channel]] */
/* Should be part of chattr */

/* matchattr <handle> <flags> [channel] */
int egg_matchattr(struct network *net, const char *handle, const char *flags, const char *channel)
{
  struct user *user;
  int i;
  
  /* First find the user */
  for(user=net->users;user != NULL && strcmp(handle,user->username);user=user->next);

  if (user == NULL)
    return 0;

  /* Deal with this either as a channel or global, not sure if this should
   * handle eggdrop notation a|f or whatever.
   * FIXME
   */
  if (channel != NULL)
  {
  } 
  else
  {
    if (user->flags == NULL)
      return 0;

    /* No channel, just check global flags */
    for (i=0;*(flags+i) != '\0';i++)
    {
      if (strchr(user->flags,*(flags+i)) == NULL)
        return 0;
    }
  }

  return 1;
 
}

/* adduser <handle> [hostmask] */
/* addbot <handle> <address> */
/* deluser <handle> */
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
/* unstick <banmask> [channel] */
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
/* ispermban <ban> [channel] */
/* isexempt <exempt> [channel] */
/* ispermexempt <exempt> [channel] */
/* isinvite <invite> [channel] */
/* isperminvite <invite> [channel] */
/* isbansticky <ban> [channel] */
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
/* save */
/* reload */
/* backup */
/* getting-users */
/* channel add <name> <option-list> */
/* channel set <name> <options...> */
/* channel info <name> */
/* channel get <name> <setting> */
/* channel remove <name> */
/* savechannels */
/* loadchannels */
/* channels */
/* isbotnick <nick> */
/* botisop [channel] */
/* botishalfop [channel] */
/* botisvoice [channel] */
/* botonchan [channel] */
/* isop <nickname> [channel] */
/* ishalfop <nickname> [channel] */
/* wasop <nickname> <channel> */
/* washalfop <nickname> <channel> */

/* isvoice <nickname> [channel]
int egg_isvoice(struct network *net, const char *nick, const char *channel)
{
  
}*/
 
/** 
 * Eggdrop Compatible onchan
 * @param net A network struct where this is to be checked.
 * @param nickname The nickname to try and find.
 * @param channel Optional channel name to check in
 * @return 1 if user found on optional channel, 1 if user found without optional channel, 0 if user not found on optional channel or 0 if not found without optional channel
 */
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

				/* Found the channel, now find the channel user */
				cuser = chan->user_list;

				while (cuser->prev != NULL) cuser = cuser->prev;
				
				while (cuser != NULL)
				{
					if (!tstrcasecmp(cuser->nick,nickname))
						return 1; /* Found the user on the channel */
					cuser = cuser->next;
				}

				if (cuser == NULL)
					return 0; /* Found channel, but not the nick */
				
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
				cuser = chan->user_list;

        while (cuser->prev != NULL) cuser = cuser->prev;

				while (cuser != NULL)
				{
					if (!tstrcasecmp(cuser->nick,nickname))
						return 1; /* Found the user */

					cuser = cuser->next;
				}

			}
			
			chan = chan->next;
		}

		return 0; /* User not found */
	}

	return 0; /* never should be reached */
}
/* nick2hand <nickname> [channel] */
/* hand2nick <handle> [channel] */
/* handonchan <handle> [channel] */
/* ischanban <ban> <channel> */
/* ischanexempt <exempt> <channel> */
/* ischaninvite <invite> <channel> */
/* chanbans <channel> */
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
/* getchanmode <channel> */
/* jump [server [port [password]]] */
/* pushmode <channel> <mode> [arg] */
/* flushmode <channel> */
/* topic <channel> */
/* validchan <channel> */
/* isdynamic <channel> */

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
void egg_dccbroadcast(struct network *net, const char *message)
{
  struct dcc_session *dtmp;
  
  dtmp = net->dccs;

  while (dtmp != NULL)
  {
    if (dtmp->status >= DCC_NOTREADY)
    {
      irc_printf(dtmp->sock,message);
      return;
    }
  
    dtmp = dtmp->next;
  }

  return;
}

/* dccputchan <channel> <message> */
/* boot <user@bot> [reason] */
/* dccsimul <idx> <text> */

/* Returns 0 on error, an idx if successful */
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

/* valididx <idx> */
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
Description: 
  'bind' is used to attach procedures to certain events. 
  flags are the flags the user must have to trigger the event (if applicable). 
  proc-name is a pointer to the procedure to call for this command.
  If the proc-name is NULL, no binding is added. 
Returns: 
  name of the command that was added, or (if proc-name is NULL), a list of the current bindings for this command
*/
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
    else if (!strcmp("dcc",type))
    {
      trigger_list_add(&net->trigs->dcc,new_trigger(NULL,TRIG_DCC,mask,cmd,handler));
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
/* binds ?type/mask? */
/* logfile [<modes> <channel> <filename>] */
/* maskhost <nick!user@host> */
/* timer <minutes> <tcl-command> */
/* utimer <seconds> <tcl-command> */

/* timers */
/* utimers */
/* killtimer <timerID> */
/* killutimer <timerID> */

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
/* die [reason] */
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

#ifdef CLOWNS
void egg_rehash(void)
{
  struct tconfig_block *cfg;
  struct config *oldcfg;

  oldcfg = g_cfg;

  tcfg  = file_to_tconfig(NULL,filename);

  g_cfg = config_engine_load(tcfg);

  /* keep a copy in the global config */
  g_cfg->tcfg = tcfg;
 
  /* We need to match the old networks to new ones
   * in case they were renamed, deleted, or added
   * to properly disconnect or connect  
   */
}
#endif /* CLOWNS */

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

