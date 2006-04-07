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

#include "main.h"
#include "users.h"

/* Check default_handlers.c */
int msg_hello(struct irc_data *data, const char *line)
{
  /* This gets called when a user /msg <bot> hello 
   * the hostmask needs checked
   */
  
  return 1;
}

int load_userdb(void)
{
  FILE *fp;
  
  if ((fp = fopen("./users.db","r")) == NULL)
  {
    troll_debug(LOG_WARN,"Could not open users.db");
    return 0;
  }
  
  /* Load existing userdb */  

  close(fp);

  return 1;
}
