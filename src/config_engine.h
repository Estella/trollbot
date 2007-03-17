#ifndef __CONFIG_ENGINE_H__
#define __CONFIG_ENGINE_H__

#include "server.h"
#include "network.h"
#include "tconfig.h"

/* This is the main struct that information is loaded
 * into from the config block format.
 * All user contributed data goes in here.
 */
struct config
{
  struct network *network_list;
  struct network *network_head;
  struct network *network_tail;

  /* Global */
  char *g_botnick;
  char *g_realname;
  char *g_ident;

  /* Owner */
  char *owner;
  char *ohostmask;
  char *opasshash;
  
  /* For TCL */
  /* For PHP */
};

/* Function Prototypes */
int config_engine_init(char *filename);
struct config *config_engine_load(struct tconfig_block *tcfg);

#endif /* __CONFIG_ENGINE_H__ */
