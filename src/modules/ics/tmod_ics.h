#ifndef __TMOD_ICS_H__
#define __TMOD_ICS_H__

struct tconfig_block;
struct slist;

#define tmodule_load          ics_LTX_tmodule_load
#define tmodule_unload        ics_LTX_tmodule_unload
#define tmodule_get_tsockets  ics_LTX_tmodule_get_tsockets
#define tsockets              ics_LTX_tsockets
#define ics_servers           ics_LTX_ics_servers

extern struct slist *tsockets;
extern struct slist *ics_servers;

struct slist *tmodule_get_tsockets(void);
int tmodule_load(struct tconfig_block *tcfg);
int tmodule_unload(void);

#endif /* __TMOD_ICS_H__ */
