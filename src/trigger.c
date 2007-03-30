#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util.h"
#include "trigger.h"
#include "egglib.h"

struct trigger *new_trigger(char *flags, int type, char *mask, char *command, void (*handler)(struct network *, struct trigger *, struct irc_data *))
{
  struct trigger *ret;
  char *tmp;
  size_t size = 0;

  ret = tmalloc(sizeof(struct trigger));
   
  /* for the ease of eggdrop scripts, flags are passed as userflags|chanflags with NULL equaling - */
  if (flags != NULL)
  {
    /* If there's a user|chan pair */
    if ((tmp = strchr(flags,'|')) != NULL)
    { 
      tmp++;
      /* tmp should now contain chanflags
       * strncpy(dst,flags,strlen(flags) - (strlen(tmp) + 1))
       * to get user flags
       */
      if (tmp != NULL)
        ret->chan_flags = tstrdup(tmp);
      else
        ret->chan_flags = NULL;

      size = strlen(flags) - (strlen(tmp) + 1);

      if (size != 0)
      {
        ret->glob_flags = tmalloc0(size+1);

        strncpy(ret->glob_flags,flags,size);
      } 
      else
        ret->glob_flags = NULL;
    }
    else
    {
      ret->chan_flags = NULL;

      /* There is no |, assume all flags are global */
      if (flags[0] == '-')
        ret->glob_flags = NULL;
      else
        ret->glob_flags = tstrdup(flags);
    }
  } 
  else
  {
    ret->glob_flags = NULL;
    ret->chan_flags = NULL;
  }

  
  ret->type    = type;
  ret->mask    = tstrdup(mask);
  ret->command = tstrdup(command);
  ret->handler = handler;

  ret->prev    = NULL;
  ret->next    = NULL;

  return ret;
}

/* This is the heart of the scripting system */
void trigger_match(struct network *net, struct irc_data *data)
{
  struct trigger *trig;

  /* First let's determine what trigger table to check */
  if (!strcmp(data->command,"PRIVMSG"))
  {
    /* Could be PUB, PUBM, MSG, MSGM atm */
    if (data->c_params[0] != NULL)
    {
      if (!strcmp(data->c_params[0],net->nick))
      {
        /* Trigger is either MSG or MSGM */
        trig = net->trigs->msg_head;

        while (trig != NULL)
        {
          if (data->rest[0] != NULL)
          {
            if (!strcmp(data->rest[0],trig->mask))
            {
              if (trig->handler != NULL)
                trig->handler(net,trig,data);
            }
          }

          trig = trig->next;
        }          


        trig = net->trigs->msgm_head;

        while (trig != NULL)
        {
          if (data->rest[0] != NULL)
          {
            if (!egg_matchwilds(data->rest_str,trig->mask))
            {
              if (trig->handler != NULL)
                trig->handler(net,trig,data);
            }

          }

          trig = trig->next;
        }

      }
      else 
      {
        trig = net->trigs->pub_head;
 
        while (trig != NULL)
        {
          if (data->rest[0] != NULL)
          {
            if (!strcmp(data->rest[0],trig->mask))
            { 
              if (trig->handler != NULL)
                trig->handler(net,trig,data);
            }
        
          }
  
          trig = trig->next;
        }

        trig = net->trigs->pubm_head;

        while (trig != NULL)
        {
          if (data->rest[0] != NULL)
          {
            if (!egg_matchwilds(data->rest_str,trig->mask))
            {
              if (trig->handler != NULL)
                trig->handler(net,trig,data);
            }

          }

          trig = trig->next;
        }
 
      }
      /* Need to handle PUBM */
    }
 
  } 
  else if (!strcmp(data->command,"NOTICE"))
  {
    /* Command is a NOTC */
  }
 
  return;

}

struct trig_table *new_trig_table(void)
{
  struct trig_table *ret;

  ret = tmalloc(sizeof(struct trig_table));

  ret->pub       = NULL;
  ret->pub_head  = NULL;
  ret->pub_tail  = NULL;
  ret->pubm      = NULL;
  ret->pubm_head = NULL;
  ret->pubm_tail = NULL;
  ret->msg       = NULL;
  ret->msg_head  = NULL;
  ret->msg_tail  = NULL;
  ret->msgm      = NULL;
  ret->msgm_head = NULL;
  ret->msgm_tail = NULL;
  ret->join      = NULL;
  ret->join_head = NULL;
  ret->join_tail = NULL;
  ret->part      = NULL;
  ret->part_head = NULL;
  ret->part_tail = NULL;
  ret->quit      = NULL;
  ret->quit_head = NULL;
  ret->quit_tail = NULL;
  ret->notc      = NULL;
  ret->notc_head = NULL;
  ret->notc_tail = NULL;

  return ret;
}

