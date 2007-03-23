#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util.h"
#include "trigger.h"
#include "trig_table.h"

struct trigger *new_trigger(char *flags, char *mask, char *command, void (*handler)(struct trigger *, struct irc_data *))
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
        trig = net->trigs->msg;

        while (trig != NULL)
        {
          if (data->rest[0] != NULL)
          {
            if (!strcmp(data->rest[0],trig->mask))
            {
              if (trig->handler != NULL)
                trig->handler(net,data);
            }
          }

          trig = trig->next;
        }
          
      }
      else 
      {
        trig = net->trigs->pub;
 
        while (trig != NULL)
        {
          if (data->rest[0] != NULL)
          {
            if (!strcmp(data->rest[0],trig->mask))
            { 
              if (trig->handler != NULL)
                trig->handler(net,data);
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
