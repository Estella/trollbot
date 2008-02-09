#include "config.h"

/* I have to do this */
#ifdef HAVE_PYTHON
#include "python_embed.h"
#endif /* HAVE_PYTHON */

#include "main.h"
#include "network.h"
#include "server.h"
#include "channel.h"
#include "user.h"
#include "trigger.h"
#include "dcc.h"

#ifdef HAVE_TCL
#include "tcl_embed.h"
#endif /* HAVE_TCL */

#ifdef HAVE_PERL
#include "perl_embed.h"
#endif /* HAVE_PERL */

#ifdef HAVE_JS
#include "js_embed.h"
#endif /* HAVE_JS */

void free_networks(struct network *net)
{
  struct network *ntmp=NULL;

  if (net == NULL)
    return;

  while (net->prev != NULL)
    net = net->prev;

  while (net != NULL)
  {
    free(net->label);
    free(net->botnick);
    free(net->nick);
    free(net->altnick);
    free(net->ident);
    free(net->realname);
    free(net->vhost);
    free(net->shost);
 
    free_users(net->users);

    free_channels(net->chans);

    free_dcc_sessions(net->dccs);

    ntmp = net;
    net  = net->next;
    free(ntmp);
  }

  return;
}
 

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

  ret->servers       = NULL; 

  ret->chans         = NULL;

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
  ret->shost         = NULL;
 
  ret->userfile      = NULL;
  ret->chanfile      = NULL;

  ret->connect_try   = -1;

  ret->connect_delay = -1;

  ret->last_try      = -1;

  ret->dccs          = NULL;

  ret->dcc_listener  = -1;
  ret->dcc_port      = -1;

  ret->tcfg          = NULL;

  /* This is the queueing BS */
  ret->connect_delay = 10; 
  ret->connect_try   = 3;
  ret->last_try      = 0;  
  
#ifdef HAVE_TCL
  net_init_tcl(ret);
#endif /* HAVE_TCL */

#ifdef HAVE_PERL
  net_init_perl(ret);
#endif /* HAVE_PERL */

#ifdef HAVE_PYTHON
  ret->pydict = NULL;
#endif /* HAVE_PYTHON */

#ifdef HAVE_JS
	ret->cx     = NULL;
	ret->global = NULL;
	/*net_init_js(ret); */
#endif /* HAVE_JS */

  return ret;
}
    

