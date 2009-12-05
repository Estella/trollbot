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
#include "irc_proto.h"
#include "user.h"

/* Supplementary */
#include "log_entry.h"

/* Self */
#include "irc_ban.h"

/* This function allows bans to be evaluated by the bot */
int irc_ban_evaluate(struct irc_ban *ban, char *mask)
{
	struct irc_hostmask *src;
	struct irc_hostmask *dst;
	int ret = 0;
	
	if ((ban == NULL) || ban->mask == NULL || mask == NULL)
		return 0;
	
	src = irc_hostmask_parse(ban->mask);
	dst = irc_hostmask_parse(mask);

/* TODO
# Set this to 0 if you want the bot to strip '~' characters from user@hosts
# before matching them.
set strict-host 0
*/
	/* If the entire ban's mask applies to mask, return TRUE, we have a winner */
	if (!matchwilds(dst->nick,  src->nick) &&
			!matchwilds(dst->ident, src->ident) &&
			!matchwilds(dst->host,  src->host))
		ret = 1;

	free(src);
	free(dst);

	/* Resolve the hostname */
	return ret;
}

struct irc_ban *irc_ban_new(void)
{
	struct irc_ban *ban = tmalloc(sizeof(struct irc_ban));

	ban->mask       = NULL;
	ban->comment    = NULL;
	ban->expiration = 0;
	ban->created    = 0;
	ban->last_time  = 0;
	ban->creator    = NULL;

	return ban;
}

void irc_ban_free(void *data)
{
	struct irc_ban *ban = data;

	free(ban->mask);
	free(ban->comment);
	free(ban->creator);
	free(ban);

	return;
}
