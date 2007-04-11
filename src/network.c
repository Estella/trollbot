#include "main.h"
#include "network.h"
#include "server.h"
#include "channel.h"
#include "user.h"
#include "trigger.h"

#ifdef HAVE_TCL
#include "tcl_embed.h"
#endif /* HAVE_TCL */

#ifdef HAVE_PERL
#include "perl_embed.h"
#endif /* HAVE_PERL */

struct network *new_network(char *label)
{
  struct network *ret;

  ret = tmalloc(sizeof(struct network));

  if (label != NULL)
    ret->label = tstrdup(label);
  else
    ret->label = NULL;

  ret->prev          = NULL;
  ret->next          = NULL;
  ret->server_head   = NULL;
  ret->server_list   = NULL;
  ret->server_tail   = NULL; 
  ret->channel_list  = NULL;
  ret->channel_head  = NULL;
  ret->channel_tail  = NULL;
  ret->cur_server    = NULL;
  ret->sock          = -1;
  ret->status        = 0;

  ret->botnick       = NULL;
  ret->nick          = NULL;
  ret->altnick       = NULL;
  ret->realname      = NULL;
  ret->ident         = NULL;

  ret->users         = NULL;

  ret->trigs         = new_trig_table();

  ret->vhost         = NULL;
 
  ret->userfile      = NULL;
  ret->chanfile      = NULL;


  ret->connect_try   = -1;

  ret->connect_delay = -1;

  ret->last_try      = -1;

#ifdef HAVE_TCL
  net_init_tcl(ret);
#endif /* HAVE_TCL */

#ifdef HAVE_PERL
  net_init_perl(ret);
#endif /* HAVE_PERL */
  return ret;
}
    

