#include "config.h"

#ifdef HAVE_PYTHON
#include "python_embed.h"
#endif /* HAVE_PYTHON */

#include "main.h"
#include "config_engine.h"

#include "network.h"
#include "server.h"
#include "channel.h"
#include "user.h"
#include "trigger.h"

#ifdef HAVE_PHP
#include "php_embed.h"
#endif /* HAVE_PHP */


/* The configuration process works in 2 steps
 * first it loads the file's data in tree form
 * and checks for syntax, then it walks through
 * handlers to be able to work with config files
 * in an overly complex and bloated way.
 */
int config_engine_init(char *filename)
{
  struct tconfig_block *tcfg;
    
  tcfg      = file_to_tconfig(filename);

  if (tcfg == NULL)
  {
    troll_debug(LOG_ERROR,"Did you remember to rename trollbot.conf.dist to trollbot.conf and edit it?");
    exit(EXIT_FAILURE);
  }

  g_cfg = config_engine_load(tcfg);

  /* keep a copy in the global config */
  g_cfg->tcfg = tcfg;
 
  return 0;
}

struct config *config_engine_load(struct tconfig_block *tcfg)
{
  struct config *cfg;
  struct tconfig_block *topmost = tcfg;
  struct tconfig_block *search  = tcfg;
  struct network *net;
  struct server *svr;
  struct channel *chan;
#ifdef HAVE_PYTHON
  FILE *fp;
#endif /* HAVE_PYTHON */

  cfg = tmalloc(sizeof(struct config));

  cfg->networks     = NULL;
  cfg->fork         = 0;
  cfg->forked       = 0;
  cfg->debug_level  = 0;
#ifdef HAVE_PYTHON
  cfg->py_main      = NULL;
  cfg->py_main_dict = NULL;
#endif /* HAVE_PYTHON */
  g_cfg             = cfg;


  while (topmost != NULL)
  {
    /* Only two global keywords exist right now, 
     * network and owner 
     */
    if (!strcmp(topmost->key,"global"))
    {
      if (strcmp(topmost->value,"settings"))
      {
        topmost = topmost->next;
        continue;
      }
 
      search = topmost->child;
    
      while (search != NULL)
      {
        if (!strcmp(search->key,"fork"))
        {
          cfg->fork = atoi(search->value);
        }
        else if (!strcmp(search->key,"debuglevel"))
        {
          cfg->debug_level = atoi(search->value);
        }

        search = search->next;
      }
    }
    else if (!strcmp(topmost->key,"network"))
    {
      net = cfg->networks;

      if (net != NULL)
      {
        while (net->next != NULL)
          net = net->next;

        net->next       = new_network(topmost->value);
        net->next->prev = net;
        net             = net->next;
      }
      else
      {
        cfg->networks = new_network(topmost->value);
         
        net           = cfg->networks;
        net->next     = NULL;
        net->prev     = NULL;
      }

      search = topmost->child;
  
      while (search != NULL)
      {
        if (!strcmp(search->key,"nick"))
        {
          /* Use the first value (for stacking configs) */
          if (net->nick == NULL)
          {
            net->nick = tstrdup(search->value);

            if (net->botnick != NULL)
              free(net->botnick);

            net->botnick = tstrdup(search->value);
          }
        }
        else if (!strcmp(search->key,"altnick"))
        {
          /* Use first value only */
          if (net->altnick == NULL)
            net->altnick = tstrdup(search->value);
        }
#ifdef HAVE_TCL
        else if (!strcmp(search->key,"tclscript"))
        {
            if (net->tclinterp == NULL)
              continue;
        
            if (Tcl_EvalFile(net->tclinterp, search->value) == TCL_ERROR)
            {
              troll_debug(LOG_WARN,"TCL Error: %s\n",Tcl_GetStringResult(net->tclinterp));
            }
        }
#endif /* HAVE_TCL */
#ifdef HAVE_PHP
        else if (!strcmp(search->key,"phpscript"))
        {
            /* PHP only allows us a global interpreter, what a piece of shit
             * So I'll start it up if and only if a PHP script exists
             */
            myphp_eval_file(search->value);
        }
#endif /* HAVE_PHP */
#ifdef HAVE_PYTHON
        else if (!strcmp(search->key,"pythonscript") || !strcmp(search->key,"pyscript"))
        {
          cfg_init_python(cfg);

          net_init_python(cfg,net);

          if ((fp = fopen(search->value,"r")) == NULL)
          {
            troll_debug(LOG_WARN,"Could not open file %s\n",search->value);
            continue;
          }

          PyRun_File(fp, search->value, Py_file_input, net->pydict, net->pydict);
        }
#endif /* HAVE_PYTHON */
        else if (!strcmp(search->key,"userfile"))
        {
          if (net->userfile == NULL)
            net->userfile = tstrdup(search->value);
        }
        else if (!strcmp(search->key,"chanfile"))
        {
          if (net->chanfile == NULL)
            net->chanfile = tstrdup(search->value);
        }
        else if (!strcmp(search->key,"realname"))
        {
          if (net->realname == NULL)
            net->realname = tstrdup(search->value);
        }
        else if (!strcmp(search->key,"ident"))
        {
          if (net->ident == NULL)
            net->ident = tstrdup(search->value);
        }
        else if (!strcmp(search->key,"vhost"))
        {
          if (net->vhost == NULL)
            net->vhost = tstrdup(search->value);
        }
        else if (!strcmp(search->key,"server"))
        {
          if (net->servers != NULL)
          {
            svr = net->servers;

            while (svr->next != NULL)
              svr = svr->next;

            svr->next         = new_server(search->value);
            svr->next->prev   = svr;
            svr               = svr->next;
          }
          else
          {
            net->servers = new_server(search->value);
            svr          = net->servers;
            svr->prev    = NULL;
            svr->next    = NULL;
          }
        }
        else if (!strcmp(search->key,"channel"))
        {
          if (net->chans != NULL)
          {
            chan = net->chans;

            while (chan->next != NULL)
              chan = chan->next;
            
            chan->next         = new_channel(search->value);
            chan->next->prev   = chan;
            chan               = chan->next;
          }
          else
          {
            net->chans = new_channel(search->value);
            chan       = net->chans;
            chan->prev = NULL;
            chan->next = NULL;
          }
        }
        
        search = search->next;
      }
    }

    topmost = topmost->next;
  }

  return cfg;
}


