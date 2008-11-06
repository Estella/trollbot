#ifndef __PGSQL_LOG_FILTER_H__
#define __PGSQL_LOG_FILTER_H__

struct network;
struct log_filter;
struct log_entry;
struct tconfig_block;

#define pgsql_log_filter_start pgsql_log_filter_LTX_pgsql_log_filter_start
#define pgsql_log_filter_stop pgsql_log_filter_LTX_pgsql_log_filter_stop
#define pgsql_log_filter_restart pgsql_log_filter_LTX_pgsql_log_filter_restart
#define pgsql_log_filter_handler pgsql_log_filter_LTX_pgsql_log_filter_handler

void pgsql_log_filter_handler(struct network *net, struct log_filter *filter, struct log_entry *entry)
{
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

int pgsql_log_filter_start(struct tconfig_block *tcfg)
{
}

#endif /* __PGSQL_LOG_FILTER_H__ */
