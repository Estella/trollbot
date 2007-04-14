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
  
  tcfg  = file_to_tconfig(NULL,filename);
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

  cfg = tmalloc(sizeof(struct config));

  cfg->networks     = NULL;
  cfg->dccs         = NULL;
  cfg->fork         = 0;
  cfg->forked       = 0;
  cfg->debug_level  = 0;
  
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

      net->tindex = topmost;

      search = topmost->child;
  
      while (search != NULL)
      {
        if (!strcmp(search->key,"nick"))
        {
          if (net->nick != NULL)
            free(net->nick);
   
          net->nick = tstrdup(search->value);

          if (net->botnick != NULL)
            free(net->botnick);

          net->botnick = tstrdup(search->value);
        }
        else if (!strcmp(search->key,"altnick"))
        {
          if (net->altnick != NULL)
            free(net->altnick);

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
        else if (!strcmp(search->key,"userfile"))
        {
          if (net->userfile != NULL)
            free(net->userfile);
 
          net->userfile = tstrdup(search->value);
        }
        else if (!strcmp(search->key,"chanfile"))
        {
          if (net->chanfile != NULL)
            free(net->chanfile);

          net->chanfile = tstrdup(search->value);
        }
        else if (!strcmp(search->key,"realname"))
        {
          if (net->realname != NULL)
            free(net->realname);

          net->realname = tstrdup(search->value);
        }
        else if (!strcmp(search->key,"ident"))
        {
          if (net->ident != NULL)
            free(net->ident);

          net->ident = tstrdup(search->value);
        }
        else if (!strcmp(search->key,"vhost"))
        {
          if (net->vhost != NULL)
            free(net->vhost);
 
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


