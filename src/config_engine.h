#ifndef __CONFIG_ENGINE_H__
#define __CONFIG_ENGINE_H__

struct dcc_session;
struct network;
struct user;
struct tconfig_block;

/* This is the main struct that information is loaded
 * into from the config block format.
 * All user contributed data goes in here.
 */
struct config
{
  struct network     *networks;
  struct dcc_session *dccs;
  struct user        *g_users;

  /* Unhandled tconfig blocks at toplevel */
  struct tconfig_block *tcfg;
  
  /* For TCL */
  /* For PHP */
};

/* Function Prototypes */
int config_engine_init(char *filename);
struct config *config_engine_load(struct tconfig_block *tcfg);

#endif /* __CONFIG_ENGINE_H__ */
