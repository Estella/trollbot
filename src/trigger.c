#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util.h"

struct trigger
{
  char *glob_flags;
  char *chan_flags;

  /* What triggers are matched against */
  char *mask;

  void (*handler)(struct trigger *);

  char *command;

  struct trigger *prev;
  struct trigger *next;
};

struct trigger *new_trigger(char *flags, char *mask, char *command, void (*handler)(struct trigger *))
{
  struct trigger *ret;
  char *tmp;
  size_t size = 0;

  ret = tmalloc(sizeof(struct trigger));
   
  /* for the ease of eggdrop scripts, flags are passed as userflags|chanflags with NULL equaling - */
  if (flags != NULL)
  {
    /* If there's a user|chan pair */
    if ((tmp = strchr(flags,"|"))) != NULL)
    { 
      tmp++;
      /* tmp should now contain chanflags
       * strncpy(dst,flags,strlen(flags) - (strlen(tmp) + 1))
       * to get user flags
       */
      if (tmp != NULL)
      {
        ret->chan_flags = tstrdup(tmp);
      }
      else
        ret->chan_flags = NULL;

      size = strlen(flags) - (strlen(tmp) 
      /* Off by one for NULL */
      ret->glob_flags = tmalloc0(strlen(flags) - (strlen(tmp));
      strncpy(ret->glob_flags,flags,strlen(flags) - (strlen(tmp) + 1));
    
    }
      
  return ret;
}
