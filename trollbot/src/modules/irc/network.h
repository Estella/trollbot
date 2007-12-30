#ifndef __NETWORK_H__
#define __NETWORK_H__

#include "config.h"

#include <time.h>

/* All lists are at head */
struct network
{
  char *label;

  char *botnick;
  
  char *nick;
  char *altnick;
  char *ident;
  char *realname;

  char *vhost;
  char *shost; /* Server given host */
 
  /* This is to implement connection queueing 
   * connect_try is set to the amount of times 
   * try connecting before giving up, -1 if never
   */
  int connect_try;

  /* Time in seconds to wait before trying to reconnect */
  time_t connect_delay;

  /* if (connect_try--) if (last_try + connect_delay <= time(NULL)) connect() */
  time_t last_try;
};

struct network *network_new(char *label);

#endif /* __NETWORK_H__ */
