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
#include <EXTERN.h>
#include <perl.h>

#include "trollbot.h"
#include "perl_embed.h"
#include "perl_lib.h"

#include "irc_network.h"

void net_init_perl(struct network *net)
{
	net->perlinterp = perl_alloc();

	perl_construct(net->perlinterp);

	troll_debug(LOG_DEBUG,"Started perl interpreter for net (%s)",net->label);
}


