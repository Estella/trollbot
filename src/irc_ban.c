/*********************************************
 * TrollBot v1.0                             *
 *********************************************
 * TrollBot is an eggdrop-clone designed to  *
 * work with multiple networks and protocols *
 * in order to present a unified scriptable  *
 * event-based platform,                     *
 *********************************************
 * This software is PUBLIC DOMAIN. Feel free *
 * to use it for whatever use whatsoever.    *
 *********************************************
 * Originally written by poutine/DALnet      *
 *                       kicken/DALnet       *
 *                       comcor/DALnet       *
 *********************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Utility */
#include "tconfig.h"
#include "util.h"

/* Protocol */
#include "irc.h"
#include "irc_network.h"
#include "irc_channel.h"
#include "user.h"

/* Supplementary */
#include "log_entry.h"

/* Self */
#include "irc_ban.h"

struct ban *ban_new(void)
{
	struct ban *ban = tmalloc(sizeof(struct ban));

	ban->mask       = NULL;
	ban->comment    = NULL;
	ban->expiration = 0;
	ban->created    = 0;
	ban->last_time  = 0;
	ban->creator    = NULL;

	return ban;
}

void ban_free(void *data)
{
	struct ban *ban = data;

	free(ban->mask);
	free(ban->comment);
	free(ban->creator);

	return;
}
