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
#include "main.h"
#include "handlers.h"
#include "triggers.h"
#include "irc.h"

int handler_call(struct irc_data *data, struct triggers *trigger)
{
  int ret = 0;

  switch (trigger->handler->status)
  {
    case HANDLER_DISABLED:
      ret = -1;
      break;
    case HANDLER_SHUTDOWN:
      /* Start up the handler */
      if (trigger->handler->startup_func != NULL)
      {
        if ((*trigger->handler->startup_func)() == 1)
        {
          trigger->handler->status = HANDLER_READY; 
          trigger->handler->usecount++;
        } else {
          trigger->handler->status = HANDLER_DISABLED;
          return 0;
        }
      }
  
      if (trigger->handler->exec_func != NULL)
      {
        ret = (*trigger->handler->exec_func)(data,trigger->exec);
      }

      /* Don't unload handler if it's persistant */
      if (trigger->handler->persistant != 1 && trigger->handler->shutdown_func != NULL)
      {
        (*trigger->handler->shutdown_func)();
        trigger->handler->status = HANDLER_SHUTDOWN;
      }          
     
      break;
    case HANDLER_READY:
      trigger->handler->usecount++;
      ret =  (*trigger->handler->exec_func)(data,trigger->exec);

      /* Don't unload handler if it's persistant */
      if (trigger->handler->persistant != 1 && trigger->handler->shutdown_func != NULL)
      {
        (*trigger->handler->shutdown_func)();
        trigger->handler->status = HANDLER_SHUTDOWN;
      }

      break;    
  }

  return ret;
} 

struct handlers *handler_lookup(const char *handler_name)
{
  struct handlers *tmp     = NULL;

  if (handlers_head == NULL)
    return NULL;

  tmp = handlers_head;
  
  while (tmp != NULL)
  {
    if (!strcmp(handler_name,tmp->name))
      return tmp;

    tmp = tmp->next;
  }

  /* Not found */
  return NULL;
}
 

void add_handler(const char *name,
                 int (*startup_func)(void),
                 int (*exec_func)(struct irc_data *,const char *),
                 int (*shutdown_func)(void),
                 int persistant)
{
  struct handlers *tmp     = NULL;

  if (handlers == NULL)
  {
    handlers                  = tmalloc(sizeof(struct handlers));
    handlers->prev            = NULL;
    handlers_head             = handlers;
    handlers_tail             = handlers;
    tmp                       = handlers;
  } else {
    handlers_tail->next       = tmalloc(sizeof(struct handlers));
    handlers_tail->next->prev = handlers_tail;
    handlers_tail             = handlers_tail->next;
    tmp                       = handlers_tail;
  }

  tmp->name          = tstrdup(name);
  tmp->startup_func  = startup_func;
  tmp->exec_func     = exec_func;
  tmp->shutdown_func = shutdown_func;
  tmp->persistant    = persistant;
  tmp->usecount      = 0;
  tmp->status        = 0;

  tmp->next = NULL;

  if (tmp->persistant == 1)
    if (tmp->startup_func != NULL)
      if ((*tmp->startup_func)())
        tmp->status = HANDLER_READY;

  return;
}


