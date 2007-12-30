#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include <stdio.h>
#include <stdlib.h>

#ifdef HAVE_STRING_H
#include <string.h>
#endif /* HAVE_STRING_H */

#ifdef HAVE_STDARG_H
#include <stdarg.h>
#endif /* HAVE_STDARG_H */

#include <ltdl.h>

/* According to POSIX.1-2001 */
#ifdef HAVE_SYS_SELECT_H
#include <sys/select.h>
#endif /* HAVE_SYS_SELECT_H */

/* According to earlier standards */
#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif /* HAVE_SYS_TIME_H */

#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif /* HAVE_SYS_TYPES_H */

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif /* HAVE_UNISTD_H */

#include "tconfig.h"
#include "tmodule.h"
#include "loop.h"

int main(int argc, char *argv[])
{
#ifdef HAVE_WORKING_FORK
  pid_t pid; /* Guaranteed to be int on systems without pid_t done by autoconf */
#endif /* HAVE_WORKING_FORK */
  struct tconfig_block *tcfg;
  struct slist *list;
  int lt_errors;

  /* This macro let's libltdl know what preloaded symbols exist if
   * modules are statically linked
   */
  LTDL_SET_PRELOADED_SYMBOLS();

  /* Make sure libltdl initializes, otherwise this program is useless */
  if ((lt_errors = lt_dlinit()) > 0)
  {
    printf("Trollbot could not initialize libltdl, initialization failed with %d errors\n",lt_errors);
    return EXIT_FAILURE;
  }

  printf("#################################################\n");
  printf("# Trollbot v2.0.0, written by poutine/DALnet    #\n");
  printf("#################################################\n");  

  if (argc > 2)
  {
    printf("# %-45s #\n","Proper syntax: ./trollbot [config file]");
    printf("#################################################\n");
    return EXIT_FAILURE;
  }

  printf("# %-45s #\n","Reading configuration");
  printf("#################################################\n");
  printf("Configuration Parser Output:\n");

  if (argc == 2)
    tcfg = file_to_tconfig(argv[1]);
  else
    tcfg = file_to_tconfig("trollbot.conf");

  if (tcfg == NULL)
  {
    printf("# Empty configuration. Aborting.                #\n");
    printf("# %-45s #\n","Proper syntax: ./trollbot [config file]");
    printf("#################################################\n");
    return EXIT_FAILURE;
  }

  list = tmodule_load_all(tcfg);

  printf("#################################################\n");
  printf("# %-45s #\n","Configuration file looks good");
  printf("# %-45s #\n","Entering program loop");
  printf("#################################################\n");


#ifdef HAVE_WORKING_FORK
/*  pid = fork();

  if (pid != 0)
  {
    printf("Forked with pid: %d\n",pid);
    return 0;
  }*/
#endif /* HAVE_WORKING_FORK */

  trollbot_loop(tcfg,list);

  /* clean up libltdl */
  lt_dlexit();

  return 0;
}

