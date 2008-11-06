#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <libpq-fe.h>

#include "util.h"
#include "tconfig.h"

#define pgsql_log_filter_start pgsql_log_filter_LTX_pgsql_log_filter_start
#define pgsql_log_filter_stop pgsql_log_filter_LTX_pgsql_log_filter_stop
#define pgsql_log_filter_restart pgsql_log_filter_LTX_pgsql_log_filter_restart
#define pgsql_log_filter_handler pgsql_log_filter_LTX_pgsql_log_filter_handler

void pgsql_log_filter_handler(struct network *net, struct log_filter *filter, struct log_entry *entry)
{
}

int pgsql_log_filter_start(struct tconfig_block *tcfg)
{
	printf("Started\n");
}

int pgsql_log_filter_start(struct tconfig_block *tcfg)
{
}

int pgsql_log_filter_start(struct tconfig_block *tcfg)
{
}

int pgsql_log_filter_start(struct tconfig_block *tcfg)
{
}
