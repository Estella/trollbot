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
    if (net->label){  free(net->label); }
    if (net->botnick){  free(net->botnick); }
    if (net->nick){  free(net->nick); }
    if (net->altnick){  free(net->altnick); }
    if (net->ident){  free(net->ident); }
    if (net->realname){  free(net->realname); }
    if (net->vhost){  free(net->vhost); }
    if (net->shost){  free(net->shost); }
    if (net->userfile){ free(net->userfile);  }
    if (net->chanfile){ free(net->chanfile);  }
 
    free_trigger_table(net->trigs);

    free_users(net->users);

    free_channels(net->chans);

		/* FIXME: free fucking JS shit, PHP shit, etc */

    free_dcc_sessions(net->dccs);

#ifdef HAVE_TCL
		tstrfreev(net->tcl_scripts);
#endif /* HAVE_TCL */

#ifdef HAVE_JS
		tstrfreev(net->js_scripts);
    /* Cleanup Spidermonkey */
    if (net->cx != NULL){
      JS_DestroyContext(net->cx);
    }
#endif /* HAVE_JS */

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
  
	ret->handlen       = 32;

#ifdef HAVE_TCL
	ret->tcl_scripts      = NULL;
	ret->tcl_scripts_size = 0;
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

	ret->plain_cx     = NULL;
	ret->plain_global = NULL;

	ret->js_scripts   = NULL;
	ret->js_scripts_size = 0;
#endif /* HAVE_JS */

  return ret;
}
    

