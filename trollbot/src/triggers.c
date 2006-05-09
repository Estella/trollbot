/******************************
 * Trollbot                   *
 ******************************
 * Written by poutine DALnet  *
 ******************************
 * This software is public    *
 * domain. Free for any use   *
 * whatsoever.                *
 ******************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>

#include "main.h"
#include "handlers.h"
#include "triggers.h"
#include "irc.h"
#include "tconfig.h"

/* There should probably be a head for the trigger types -poutine */
int match_triggers(struct irc_data *data)
{
  struct triggers *local;

  switch (data->bind_hint)
  {
    case RAW:
      if (trig_raw == NULL)
        break;
      local = trig_raw;
      
      REWIND_LIST(local);
      while (local != NULL)
      {
        if (!strcmp(local->trigger,data->command))
        {
          handler_call(data,local);
          break;
        }

        local = local->next;
      }

      break;     
    case CTCP:
      if (trig_ctcp == NULL)
        break;
      local = trig_ctcp;

      REWIND_LIST(local);
      while (local != NULL)
      {
        if (!strncmp(local->trigger,data->rest_str,strlen(local->trigger)))
        {
          handler_call(data,local);
          break;
        }

        local = local->next;
      }

      break;
    case PUB:
      if (trig_pub == NULL)
          break;
      local = trig_pub;
      
      REWIND_LIST(local);
      while (local != NULL)
      {
        if (!strncmp(local->trigger,data->rest_str,strlen(local->trigger)))
        {
          handler_call(data,local);
          break;
        }
  
        local = local->next;
      }
      break;
    case MSG:
      printf("We get in the message\n");
      if (trig_msg == NULL)
        break;
     
      local = trig_msg;
     
      while (local != NULL)
      {
        printf("%s\n",local->trigger);
        if (!strncmp(local->trigger,data->rest_str,strlen(local->trigger)))
        {
          handler_call(data,local);
          break;
        }
   
        local = local->next;
      }

      break;
  }

  return 0;
}
                   

void add_trigger(int type,
                 const char *trigger,
                 const char *handler_name,
                 const char *flags,
                 const char *exec)
{
  struct triggers *tmp     = NULL;
  struct handlers *handler = NULL;

  if ((handler = handler_lookup(handler_name)) == NULL)
  {
    troll_debug(LOG_WARN,"Could not find handler id %s\n",handler_name);
    return;
  }

  switch (type)
  {
    case PUB:
      tmp  = trig_pub;
      break;
    case MSG:
      tmp = trig_msg;
      break;
    case NOTC:
      tmp   = trig_notc;
      break;
    case CTCP:
      tmp   = trig_ctcp;
      break;
    default:
      tmp   = trig_raw;
      break;
  }

  if (tmp == NULL)
  {
    tmp                  = tmalloc(sizeof(struct triggers));
    tmp->prev            = NULL;
  } else {
    FASTFORWARD_LIST(tmp);
    tmp->next       = tmalloc(sizeof(struct triggers));
    tmp->next->prev = tmp;
    tmp             = tmp->next;
  }

  tmp->type    = type;
  tmp->trigger = tstrdup(trigger);
  tmp->handler = handler;

  if (flags != NULL)
    tmp->flags   = tstrdup(flags);

  tmp->exec    = tstrdup(exec);

  tmp->next = NULL;

  /*REWIND_LIST(tmp);*/

  switch (type)
  {
    case PUB:
      trig_pub = tmp;
      break;
    case MSG:
      trig_msg = tmp;
      break;
    case NOTC:
      trig_notc = tmp;
      break;
    case CTCP:
      trig_ctcp = tmp;
      break;
    default:
      trig_raw = tmp;
      break;
  }

  return;
}


