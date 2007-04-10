#include "main.h"
#include "egg_lib.h"
#include "network.h"
#include "server.h"
#include "channel.h"
#include "user.h"
#include "irc.h"

/* This is the eggdrop core API that is exported to TCL, PHP, perl, etc */


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

/* Fully compatible [QUIRK: Works per network] */
int egg_countusers(struct network *net)
{
  struct user *user;
  int count = 0;  
/*
  if (net->users_head == NULL)
    return 0;

  user = net->users_head;

  while (user != NULL)
  {
    count++;
    user = user->next;
  }

  return count;
*/ return 0; /* FIXME */
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
/* Returns: the handle found, or "*" if none */
char *egg_finduser(struct network *net, const char *mask)
{
  struct user *user;

  if ((user = net->users) == NULL)
    return 0;

  while (user != NULL)
  {
    /*if ()
      return user->username;*/
    user = user->next;
  }

  return "*";
}




/* Needs returns checked */
/* userlist [flags] */

int egg_passwdok(struct network *net, const char *handle, const char *pass) 
{
  SHA1_CTX context;
  unsigned char digest[20];

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
      SHA1Final(digest, &context);
 
      /* Pass is all good baby */
      if (!strcmp(user->passhash,digest))
        return 0;

      return 1;
    }
    user = user->next;
  }

  return 0;
}

/* getuser <handle> <entry-type> [extra info] */
/* setuser <handle> <entry-type> [extra info] */
/* chhandle <old-handle> <new-handle> */
/* chattr <handle> [changes [channel]] */
/* botattr <handle> [changes [channel]] */
/* matchattr <handle> <flags> [channel] */
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
/* isvoice <nickname> [channel] */
/* onchan <nickname> [channel] */
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
/* putdcc <idx> <text> */
/* dccbroadcast <message> */
/* dccputchan <channel> <message> */
/* boot <user@bot> [reason] */
/* dccsimul <idx> <text> */
/* hand2idx <handle> */
/* idx2hand <idx> */
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
/* bind <type> <flags> <keyword/mask> [proc-name] */
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
/* unixtime */
/* duration <seconds> */
/* strftime <formatstring> [time] */
/* ctime <unixtime> */
/* myip */
/* rand <limit> */
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
/* rehash */
/* botnick */
/* botname */
/* version */
/* numversion */
/* uptime */
/* server-online */
/* lastbind */
/* isjuped */
/* handlen */
/* config */

