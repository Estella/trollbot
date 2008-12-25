#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif /* HAVE_SYS_TYPES_H */

#include <ltdl.h>

#ifdef UNIX
#include <unistd.h>
#endif /* UNIX */

#include "config.h"

#include "main.h"
#include "sockets.h"

struct config *g_cfg = NULL;

int main(int argc, char *argv[])
{
	pid_t pid;
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
	printf("# Trollbot v1.0.0, written by                   #\n");
	printf("#################################################\n");
	printf("# poutine/DALnet - Lead Developer               #\n");
	printf("# kicken/DALnet  - Javascript Support and Core  #\n");
	printf("# comcor/DALnet  - Python Support               #\n");
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
		config_engine_init(argv[1]);
	else
		config_engine_init("trollbot.conf");

	printf("#################################################\n");
	printf("# %-45s #\n","Configuration file looks good.");
	printf("# %-45s #\n","Dumping formatted tconfig tree.");
	printf("# %-45s #\n","Entering Socket loop");
	printf("#################################################\n");

	/* Outputs a formatted tconfig tree for debugging purposes */
	tconfig_to_file(g_cfg->tcfg,"out.txt");

#ifdef HAVE_WORKING_FORK
	if (g_cfg->fork == 1)
	{
		g_cfg->forked = 1;

		pid = fork();

		if (pid != 0)
		{
			printf("Forked with pid: %d\n",pid);
			return 0;
		}
	}
#endif /* HAVE_WORKING_FORK */

	/*while(1)
	{
	}*/

	socket_loop();

	die_nicely(EXIT_SUCCESS);

	return 0;
}
