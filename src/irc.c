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
#include <stdarg.h>

#include "main.h"
#include "irc.h"
#include "network.h"
#include "channel.h"
#include "trigger.h"
#include "dcc.h"

/* Simple printf like function that outputs to a socket, buffer work needs to be more dynamic */
/* Should be sock_printf() */
void irc_printf(int sock, const char *fmt, ...)
{
  va_list va;
  char buf[2048];
  char buf2[2059];

  memset(buf, 0, sizeof(buf));
  memset(buf2, 0, sizeof(buf2));

  va_start(va, fmt);

  /* C99 */
  vsnprintf(buf, sizeof(buf), fmt, va);
  va_end(va);

  snprintf(buf2,sizeof(buf2),"%s\n",buf);

  send(sock,buf2,strlen(buf2),0);
}

/* Constructor */
struct irc_data *irc_data_new(void)
{
  struct irc_data *local;

  local = tmalloc(sizeof(struct irc_data));

  local->prefix        = NULL;
  local->command       = NULL;
  local->c_params      = NULL;
  local->rest          = NULL;

  local->c_params_str  = NULL;
  local->rest_str      = NULL;

  return local;
}

/* Destructor */
void irc_data_free(struct irc_data *data)
{
  if (data->prefix != NULL)
  {
    free(data->prefix->servername);
    free(data->prefix->nick);
    free(data->prefix->user);
    free(data->prefix->host);

    free(data->prefix);
  }

  free(data->command);
  free(data->c_params_str);
  free(data->rest_str);

  /* Frees initial pointer also */
  tstrfreev(data->c_params);
  tstrfreev(data->rest);

  free(data);
}

/* This function gets an unparsed line from IRC, and makes it into the irc_data struct */
void parse_irc_line(struct network *net, const char *buffer)
{
  struct irc_data *data    = NULL;
  char            *tmp     = NULL;
  int             which    = 0,
                  i        = 0,
                  j        = 0,
                  m        = 0,
                  bufindex = 0;
  unsigned int    bufsize  = 0;

  data = irc_data_new();

  if (buffer[0] != ':')
  {
    /* No prefix */
    data->prefix = NULL;
  } else {
    data->prefix = tmalloc(sizeof(struct irc_prefix));

    /* For storing the temp prefix */
    tmp          = tmalloc0(strlen(buffer) + 1);

    for (i=0; buffer[i] != ' ';i++)
    {
      if (buffer[i] == '!')
        which = 1;

      tmp[i] = buffer[i];
    }

    /* If which == 1 at this point, the prefix is
     * :nick!user@host
     * else it is
     * :servername
     *
     * Exception:
     *   if :<nick>, then user command
     */

    if (which == 0)
    {
      /* skip trailing : */
      data->prefix->servername = tstrdup(&tmp[1]);
      data->prefix->nick       = NULL;
      data->prefix->user       = NULL;
      data->prefix->host       = NULL;

      /* So we know where we're at */
      m                        = strlen(tmp)+1;

      /* If ServerName == Bot's nick, do a swap */
      if (!strcmp(data->prefix->servername,net->nick))
      {
        data->prefix->nick       = data->prefix->servername;
        data->prefix->servername = NULL;
      }
    } else {
      data->prefix->servername = NULL;
      data->prefix->nick       = tmalloc0(strlen(tmp) + 1);
      data->prefix->user       = tmalloc0(strlen(tmp) + 1);
      data->prefix->host       = tmalloc0(strlen(tmp) + 1);

      for(i=1,j=0;tmp[i] != '!';i++, j++)
        data->prefix->nick[j]  = tmp[i];

      for(i++, j=0;tmp[i] != '@';i++, j++)
        data->prefix->user[j]  = tmp[i];

      for(i++, j=0;tmp[i] != '\0';i++, j++)
        data->prefix->host[j]  = tmp[i];

      /* set our current position in m */
      m = i+1;
    }

    free(tmp);
  } /* Prefix is now all taken care of */

  data->command = tmalloc0(strlen(&buffer[m]) + 1);

  for(j=0;buffer[m] != ' ';m++, j++)
    data->command[j] = buffer[m];

  m++;

  /* Fill in command parameters if any */
  for(i=0,j=0;buffer[m] != '\0' && buffer[m] != '\n' && buffer[m] != '\r';m++, j++, i++)
  {
    if (buffer[m] == ':' && buffer[m-1] == ' ')
      break;

    if (data->c_params == NULL)
    {
      /* Allocate 9 usable paramaters, keep 10 NULL */
      bufsize            = sizeof(char *) * 10;
      bufindex           = 0;
      data->c_params     = tmalloc0(bufsize);

      data->c_params[0]  = tmalloc0(strlen(&buffer[m]) + 1);

      data->c_params_str = tmalloc0(strlen(&buffer[m]) + 1);
    }

    if (buffer[m] == ' ')
    {
      bufindex++;

      /* We're on the last slot that should be marked NULL if true */
      if (((bufindex+1) * sizeof(char *)) == bufsize)
      {
        /* allocate 10 more slots */
        data->c_params = tsrealloc0(data->c_params,
                                    sizeof(char *) * (bufindex + 10 + 1),
                                    &bufsize);

      }

      m++;
      data->c_params_str[i] = ' ';
      i++;

      if (buffer[m] == ':' || buffer[m] == '\r' || buffer[m] == '\n')
        break;

      j = 0;

      data->c_params[bufindex]   = tmalloc0(strlen(&buffer[m]) + 1);
			data->c_params[bufindex+1] = NULL;
    }

    data->c_params[bufindex][j] = buffer[m];
    data->c_params_str[i]       = buffer[m];
  }

  data->rest = NULL;

  if (buffer[m] != '\0')
    m += 1; /* Skip ':' */

  for(j=0,i=0;buffer[m] != '\0' && buffer[m] != '\n' && buffer[m] != '\r';m++, j++, i++)
  {
    if (data->rest == NULL)
    {
      bufsize            = sizeof(char *) * 10;
      data->rest         = tmalloc0(bufsize);
      bufindex           = 0;

      data->rest[0]      = tmalloc0(strlen(&buffer[m]) + 1);
      data->rest_str     = tmalloc0(strlen(&buffer[m]) + 1);
    }

    if (buffer[m] == ' ')
    {

      bufindex++;
      if (((bufindex+1) * sizeof(char *)) == bufsize)
      {
        /* Needs changed */
        data->rest = tsrealloc0(data->rest,
                                sizeof(char *) * (bufindex + 10 + 1),
                                &bufsize);
      }

      m++;
      data->rest_str[i] = ' ';
      i++;

      if (buffer[m] == '\r' || buffer[m] == '\n' || buffer[m] == '\0')
        break;

      j = 0;

      data->rest[bufindex] = tmalloc0(strlen(&buffer[m]) + 1);
			data->rest[bufindex+1] = NULL;
    }

    data->rest[bufindex][j] = buffer[m];
    data->rest_str[i]       = buffer[m];
  }

  /* That's all for now */
  if (data->prefix != NULL)
  {
    if (data->prefix->servername != NULL)
    {
      troll_debug(LOG_DEBUG,"Servername: %s",data->prefix->servername);
    } else {
      if (data->prefix->nick != NULL)
        troll_debug(LOG_DEBUG,"Nick: %s",data->prefix->nick);
      if (data->prefix->user != NULL)
        troll_debug(LOG_DEBUG,"User: %s",data->prefix->user);
      if (data->prefix->host != NULL)
        troll_debug(LOG_DEBUG,"Host: %s",data->prefix->host);
    }
  }

  troll_debug(LOG_DEBUG,"Command: %s",data->command);

  if (data->c_params != NULL)
    troll_debug(LOG_DEBUG,"Command Parameters: %s",data->c_params_str);

  if (data->rest != NULL)
    troll_debug(LOG_DEBUG,"Rest: %s",data->rest_str);

  /* deal with pings */
  if (!strcmp(data->command,"PING"))
  {
    if (data->rest[0] != NULL)
      irc_printf(net->sock, "PONG :%s\n",data->rest[0]);
  }

  /* Deal with end of MOTD to join channels */
  if (!strcmp("376",data->command))
  {
    irc_printf(net->sock,"USERHOST %s",net->nick);
    join_channels(net);
    net->status = STATUS_IDLE;
    
  }

  /* Deal with ERR_NICKNAMEINUSE
   * this should be in default_triggers,
   * but there's no raw trigger type yet
   */
  /* Also needs bind time event for changing
   * nickname back to regular nick. Perhaps
   * even a custom event for nickserv ghost and
   * such.
   */
  if (!strcmp("433",data->command))
  {
    if (net->altnick != NULL)
      irc_printf(net->sock,"NICK %s",net->altnick);
    
    if (net->botnick != NULL)
      free(net->botnick);

    /* Should just make this a pointer to nick
     * or altnick
     */
    net->botnick = tstrdup(net->altnick);
  }

  /* 302, Ip? */
  if (!strcmp("302",data->command))
  {    
    if (net->shost == NULL)
    {
      if (data->rest_str != NULL)
      {
        /* I think I can reliably use this to get the IP from server */
        if ((tmp = strchr(data->rest_str,'@')) != NULL)
        {
          tmp++; /* Skip over @ */
          net->shost = tstrdup(tmp);
          if (net->shost != NULL)
            dcc_init_listener(net);
          
        }
      }
    }
  }

  /* Go through triggers for the net */
  trigger_match(net,data);

  irc_data_free(data);

  /* Get yourself an aspirin */
}

int irc_in(struct network *net)
{
  static char         *buffer  = NULL;
  static size_t       size     = BUFFER_SIZE;
  int                 recved   = 0;
  char                *line    = NULL;
  const char          *ptr     = NULL;
  char                *optr    = NULL;
  char                *bufcopy = NULL;

  if (buffer == NULL)
  {
    buffer = tmalloc0(BUFFER_SIZE + 1);
    recved = recv(net->sock,buffer,BUFFER_SIZE-1,0);
  } else {
    /* There was a fragment left over */
    buffer = tcrealloc0(buffer,
                        strlen(buffer) + BUFFER_SIZE + 1,
                        &size);

    recved = recv(net->sock,&buffer[strlen(buffer)],BUFFER_SIZE-1,0);

  }


  switch (recved)
  {
    case -1:
      free(buffer);
      buffer = NULL;
      return 1;
    case 0:
      net->sock = -1;
      return 0;
  }

  while (strchr(buffer,'\n') != NULL)
  { /* Complete IRC line */
    line = tmalloc0(strlen(buffer)+1);

    optr = line;

    for(ptr = buffer;*ptr != '\n' && *ptr != '\r';ptr++)
    {
      *optr = *ptr;
      optr++;
    }

    /* This should deal with ircds which output \r only, \r\n, or \n */
    while (*ptr == '\r' || *ptr == '\n')
      ptr++;

    parse_irc_line(net,line);

    free(line);

    if (strlen(ptr) == 0)
    {
      free(buffer);
      buffer = NULL;
      break;
    }

    bufcopy = tstrdup(ptr);

    free(buffer);

    size   = strlen(bufcopy) + 1;

    buffer = bufcopy;
  }

  return 1;
}
