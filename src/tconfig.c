/******************************
 * Trollbot                   *
 ******************************
 * Written by poutine DALnet  *
 ******************************
 * This software is public    *
 * domain. Free for any use   *
 * whatsoever.                *
 ******************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "main.h"
#include "tconfig.h"

void parse_config(void)
{
  FILE *cfile;
  char buffer[BUFFER_SIZE];

  /* No memory allocated at this point, we don't need 
   * to die nicely
   */

  if ((cfile = fopen("trollbot.conf","r")) == NULL)
  {
    /* troll_debug requires config parse ?why? */
    printf("Could not open trollbot.conf\n");
    exit(1);
  }

  /* Initialize parent structure */
  glob_config_new();

  while (!feof(cfile))
  {
    memset(buffer,0,sizeof(buffer));
    
    fgets(buffer,BUFFER_SIZE,cfile);

    if (buffer[0] != '\n' && buffer[0] != '#' && 
        buffer[0] != '\r' && strlen(buffer) > 0)
    {
      parse_config_line(buffer);
    }
  }

  close(cfile);
}

void parse_config_line(char *buffer)
{
  char *lvalue = NULL;
  char *rvalue = NULL;
  const char *local  = buffer;
  int  i       = 0,
       j       = 0;  

    lvalue = buffer;
    //Find the = sign if it exists.
    if ((rvalue = strchr(buffer, '=')) == NULL)
        return;

    //rvalue should point to the = sign now.
    //Null the = so that it becomes the terminiator for lvalue
    //+1 the rvalue so it starts at the character after the =
    *rvalue = '\0';
    rvalue++;

    //Remove any spaces/newlines
    lvalue = tstrtrim(lvalue);
    rvalue = tstrtrim(rvalue);



  /* Rvalue and Lvalue should be good */
  if (!strcmp(lvalue,"chan"))
  {
    add_channel(rvalue);

  } else if (!strcmp(lvalue,"server")) {

    add_server(rvalue);

  } else if (!strcmp(lvalue,"ircnick")) {

    set_nick(rvalue);

  } else if (!strcmp(lvalue,"altnick")) {

    set_altnick(rvalue);

  } else if (!strcmp(lvalue,"sendq")) {
 
    set_sendq(rvalue);
 
  } else if (!strcmp(lvalue,"fork")) {
 
    set_fork(atoi(rvalue));
 
  } else if (!strcmp(lvalue,"debug")) {
 
    set_debug(atoi(rvalue));

  } else if (!strcmp(lvalue,"tclscript")) {
     
    add_tcl_script(rvalue);

  } else if (!strcmp(lvalue,"phpscript")) {
  
    add_php_script(rvalue);

  } else if (!strcmp(lvalue,"vhost")) {
     
    set_vhost(rvalue);

  } else {

    troll_debug(LOG_WARN,"Invalid Configuration variable (%s)",lvalue);

  }

  return;
}

void set_vhost(const char *vhost)
{
  if (config->vhost != NULL)
    free(config->vhost);

  config->vhost = tstrdup(vhost);
}

void set_fork(int flag)
{
  config->fork = flag;
}

void set_debug(int level)
{
  config->debug = level;
}

void set_nick(const char *nick)
{
  if (config->nick != NULL)
    free(config->nick);

  config->nick = tstrdup(nick);
  
  return;
}

void set_altnick(const char *nick)
{
  if (config->altnick != NULL)
    free(config->altnick);

  config->altnick = tstrdup(nick);

  return;
}

/* Not Implemented Yet */
void set_sendq(const char *sendq)
{
  return;
}

void glob_config_new(void)
{
  config = tmalloc(sizeof(struct glob_config));
  
  config->sock      = -1;
  config->nick      = NULL;
  config->altnick   = NULL;
  config->ident     = NULL;
  config->realname  = NULL;
  config->server    = NULL;
  config->vhost     = NULL;

  config->fork      = 0;
  config->debug     = 0;
}

void glob_config_free(void)
{
  if (config != NULL)
  {
    free(config->nick);
    free(config->altnick);
    free(config->ident);
    free(config->realname);
    free(config->server);
    free(config->vhost);
 
    free(config);
  }

  return;
}

/* Int should be a code to signify fatal, missing, etc,
 * but I'm too lazy
 */
int glob_config_check(void)
{
  if (config->nick == NULL || glob_server == NULL)
     die_nicely();
  
  return 0; /* never gets here */
}

