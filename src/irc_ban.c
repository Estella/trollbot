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

#ifdef CLOWNS
/* This function allows bans to be evaluated by the bot */
int irc_ban_evaluate(struct irc_ban *ban, char *mask)
{
	char *match_ident = NULL;
	char *match_nick  = NULL;
	char *match_host  = NULL;
	char *match_ip    = NULL;
	char *ptr         = NULL;
	int  len          = 0;

	/* When evaluating bans we have several aspects
   * ident (fully realized or not with ~)
   * nickname
   * hostmask (Can be IP/Hostname)
   */
	/* Separate the mask into parts */
	ptr = mask;
	
	/* Returns 0 if invalid mask */
	if (strchr(mask, '!') || strchr(mask, '@'))
		return 0;
	
	/* Separating tokens are ! and @ */
	match_ident = tmalloc0(strlen(mask) + 1);
	match_nick  = tmalloc0(strlen(mask) + 1);
	match_host  = tmalloc0(strlen(mask) + 1);

	while (*ptr != '!')
	{
		match_ident = *ptr;
		ptr++;
	}

	/* Resolve the hostname */
	return 0;
}
#endif /* CLOWNS */

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
