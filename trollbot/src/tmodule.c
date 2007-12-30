#ifdef CONFIG_H
#include "config.h"
#endif /* CONFIG_H */

#include <stdio.h>
#include <stdlib.h>

#include <ltdl.h>

#include "debug.h"
#include "util.h"
#include "tconfig.h"
#include "tmodule.h"

static int tmodule_from_tconfig(struct tconfig_block *tcfg, void *extra)
{
  lt_dlhandle ltmodule   = NULL;
  struct slist *list     = extra;
  struct tmodule *module = NULL;
  int (*tmodule_init)(void *);

  if (strcmp(tcfg->key,"module"))
    return 0;

  /* Search modules directory */
  lt_dlsetsearchpath("modules");

  ltmodule = lt_dlopenext(tcfg->value);

  if (ltmodule == NULL)
  {
    troll_debug(LOG_WARN,"Could not open module \"%s\"",tcfg->value);
    return 0;
  }

  troll_debug(LOG_DEBUG,"Loaded module \"%s\"",tcfg->value);

  if ((tmodule_init = lt_dlsym(ltmodule,"tmodule_init")) == NULL)
  {
    troll_debug(LOG_WARN,"Could not find symbol tmodule_init in  module \"%s\"",tcfg->value);
    lt_dlclose(ltmodule);
  } 
  else
  {
    if (!(*tmodule_init)(tcfg->child))
    {
      troll_debug(LOG_WARN,"Init function for module \"%s\" failed",tcfg->value);
      lt_dlclose(ltmodule);
    }
  }

  module = tmalloc(sizeof(struct tmodule));

  module->name                 = tstrdup(tcfg->value);
  module->handle               = ltmodule;
  module->tmodule_init         = tmodule_init; 

  module->tmodule_get_readfds  = lt_dlsym(ltmodule,"tmodule_get_readfds");
  module->tmodule_get_writefds = lt_dlsym(ltmodule,"tmodule_get_writefds");

  module->tmodule_read_cb      = lt_dlsym(ltmodule,"tmodule_read_cb");
  module->tmodule_write_cb     = lt_dlsym(ltmodule,"tmodule_write_cb");

  slist_insert_next(list,NULL, module);

  return 0;
}

void tmodule_free(void *data)
{
  struct tmodule *module = data;

  free(module->name);
  free(module);
}  

/* Returns: Singly linked list of all loaded modules */
struct slist *tmodule_load_all(struct tconfig_block *tcfg)
{
  struct slist *list;

  slist_init(&list,tmodule_free);

  if (tcfg == NULL)
    return 0;

  tconfig_foreach_width(tcfg, tmodule_from_tconfig, list);

  return list;
}
