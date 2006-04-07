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
#include <sys/types.h>
#include <unistd.h>
#ifdef HAVE_TCL
#include <tcl.h>
#endif /* HAVE_TCL */

#include "main.h"
#include "tconfig.h"
#include "channels.h"
#include "servers.h"

/* Globals */
#ifdef HAVE_TCL
Tcl_Interp *tcl_interpreter;
#endif

/* Too many fucking globals */

struct global_config global_config;

extern struct users    *glob_users;


extern struct channels *glob_chan;

 
extern struct network *glob_networks;

extern struct dcc_session *glob_dcc;



struct triggers *trig_pub       = NULL,
                *trig_msg       = NULL,
                *trig_notc      = NULL,
                *trig_part      = NULL,
                *trig_join      = NULL,
                *trig_quit      = NULL,
                *trig_ctcp      = NULL,
                *trig_pubm      = NULL,
                *trig_mode      = NULL,
                *trig_msgm      = NULL, 
                *trig_raw       = NULL;

struct scripts  *tcl_scripts_head = NULL,
                *tcl_scripts_tail = NULL,
                *tcl_scripts      = NULL,
                *php_scripts_head = NULL,
                *php_scripts_tail = NULL,
                *php_scripts      = NULL;

struct handlers *handlers      = NULL,
                *handlers_head = NULL,
                *handlers_tail = NULL;

int main(int argc, char *argv[])
{
  printf("#################################################\n");
  printf("# Trollbot, written by poutine DALnet           #\n");
  printf("#################################################\n");  
  printf("# %-45s #\n","Running init");

  parse_config();
  printf("# %-45s #\n","Parsed Configuration");

  /*glob_config_check();*/
  printf("# %-45s #\n","Config File looks good");
  
  /*add_default_handlers();*/
  printf("# %-45s #\n","Added default handlers");

  printf("# %-45s #\n","Loading user file");
   
  /* Better instructions ? */
  if (!load_userdb())
  {
    printf("# %-45s #\n","No user file found, created one.");
    printf("# %-45s #\n","Once bot connects to IRC /msg him hello");
  }

  printf("# %-45s #\n","All done!");

  printf("#################################################\n");
 
  printf("# %-45s #\n","Entering IRC loop");
  
  printf("#################################################\n");

  /*if (config->fork == 1)
    if(fork()>0) exit(1);

  irc_loop();*/
  
  global_config_free();
  return 0;
}
