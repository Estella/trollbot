#ifndef __TMODULE_H__
#define __TMODULE_H__

#include <ltdl.h>

#include "util.h"

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
  int (*tmodule_init)(void *);
  int (*tmodule_unload)(void *);

  /* FD grabbing */
  struct slist *(*tmodule_get_readfds)(void);
  struct slist *(*tmodule_get_writefds)(void);
  
  /* Callbacks on ready to read socks */
  void (*tmodule_read_cb)(struct tsocket *);
  void (*tmodule_write_cb)(struct tsocket *);

  /* Messaging Subsystem */
  struct slist *(*tmodule_get_messages)(void);
  void (*tmodule_send_messages)(struct slist *);
};


struct tmodule *tmodule_new(void);

void tmodule_free(void *data);  
struct slist *tmodule_load_all(struct tconfig_block *tcfg);
/*
struct slist *tmodule_get_messages(void);
void tmodule_send_messages(struct slist *messages);
*/

#endif /* __TMODULE_H__ */
