#ifndef __TMODULE_H__
#define __TMODULE_H__

#include <ltdl.h>

#include "util.h"

struct tsocket;
struct tconfig;

enum tmodule_status
{
  TMOD_NOTLOADED = 0,
  TMOD_LOADERROR,
  TMOD_LOADED,
  TMOD_PERSISTANT
};

struct tmodule
{
  char *name;
 
  lt_dlhandle handle; /* libltdl module handle */
 
  int status;

  /* Constructor/Destructor */  
  int (*tmodule_load)(struct tconfig *);
  int (*tmodule_unload)();

  /* FD grabbing */
  struct slist *(*tmodule_get_tsockets)(void);
};


struct tmodule *tmodule_from_tconfig(struct tconfig_block *tcfg);
struct tmodule *tmodule_new(void);
void tmodule_free(void *data);  
struct slist *tmodule_load_all(struct tconfig_block *tcfg);

#endif /* __TMODULE_H__ */
