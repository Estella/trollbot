#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "main.h"
#include "config_engine.h"
#include "tconfig.h"
#include "network.h"
#include "server.h"
#include "channel.h"
#include "util.h"
#include "trigger.h"
#include "config.h"

#ifdef HAVE_TCL
#include <tcl.h>
#include "tcl_embed.h"
#endif /* HAVE_TCL */

#ifdef HAVE_PHP
#include <php.h>
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
  struct        config *cfg;


  tcfg  = file_to_tconfig(filename);
  cfg   = config_engine_load(tcfg);

  /* Free the old tconfig system */
  free_tconfig(tcfg);
 
  return 0;
}

struct config *config_engine_load(struct tconfig_block *tcfg)
{
  struct config *cfg;
  struct tconfig_block *topmost = tcfg;
  struct tconfig_block *search  = tcfg;
  struct network *net;
  struct server *svr;

  cfg = tmalloc(sizeof(struct config));

  g_cfg = cfg;

  cfg->network_list = NULL;
  cfg->network_head = NULL;
  cfg->network_tail = NULL;

  cfg->g_nick       = NULL;
  cfg->g_altnick    = NULL;
  cfg->g_realname   = NULL;
  cfg->g_ident      = NULL;

  cfg->owner        = NULL;
  cfg->ohostmask    = NULL;
  cfg->opasshash    = NULL;
  
  do
  {
    /* Only two global keywords exist right now, 
     * network and owner 
     */
    if (!strcmp(topmost->key,"network"))
    {
      if (cfg->network_list != NULL)
      {
        cfg->network_tail->next       = new_network(topmost->value);
        cfg->network_tail->next->prev = cfg->network_tail;
        cfg->network_tail             = cfg->network_tail->next;
      } else {
        cfg->network_list       = new_network(topmost->value);
        cfg->network_list->next = NULL;
        cfg->network_list->prev = NULL;
        cfg->network_head       = cfg->network_list;
        cfg->network_tail       = cfg->network_list;
      }

      net    = cfg->network_tail;
      search = topmost->child;
  
      do
      {
        if (!strcmp(search->key,"nick"))
        {
          if (net->nick != NULL)
            free(net->nick);
   
          net->nick = tstrdup(search->value);
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
              printf("TCL Error: %s\n",Tcl_GetStringResult(net->tclinterp));
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
        else if (!strcmp(search->key,"server"))
        {
          if (net->server_list != NULL)
          {
            net->server_tail->next         = new_server(search->value);
            net->server_tail->next->prev   = net->server_tail;
            net->server_tail               = net->server_tail->next;
          } 
          else 
          {
            net->server_list = new_server(search->value);
            net->server_list->next = NULL;
            net->server_list->prev = NULL;
            net->server_head = net->server_list;
            net->server_tail = net->server_list;
          }

          svr = net->server_tail;
        }
        else if (!strcmp(search->key,"channel"))
        {
          if (net->channel_list != NULL)
          {
            net->channel_tail->next         = new_channel(search->value);
            net->channel_tail->next->prev   = net->channel_tail;
            net->channel_tail               = net->channel_tail->next;
          }
          else
          {
            net->channel_list = new_channel(search->value);
            net->channel_list->next = NULL;
            net->channel_list->prev = NULL;
            net->channel_head = net->channel_list;
            net->channel_tail = net->channel_list;
          }
        }

      } while ((search = search->next) != NULL);
    }
    else if (!strcmp(topmost->key,"owner"))
    {
    }
  } while ((topmost = topmost->next) != NULL);

  /* Fill out global config vars */
  net = cfg->network_head;

  do
  {
    if (!strcmp(net->label,"global"))
    {
      if (net->nick != NULL)
      {
        cfg->g_nick = net->nick;
        net->nick   = NULL;
      }

      if (net->ident != NULL)
      {
        cfg->g_ident = net->ident;
        net->ident   = NULL;
      }

      if (net->realname != NULL)
      {
        cfg->g_realname = net->realname;
        net->realname   = NULL;
      }
 
      break;
    }
  } while ((net = net->next) != NULL);

  net = cfg->network_head;

  do
  {
    if (strcmp(net->label,"global"))
    {
      if (net->nick == NULL)
        net->nick = tstrdup(cfg->g_nick);

      if (net->ident == NULL)
        net->ident = tstrdup(cfg->g_ident);

      if (net->realname == NULL)
        net->realname = tstrdup(cfg->g_realname);

      break;
    }
  } while ((net = net->next) != NULL);


  return cfg;
}


