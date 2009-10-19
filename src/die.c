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
#include "trollbot.h"
#include "die.h"
#include "tconfig.h"
#include "config_engine.h"
#include "dcc.h"
#include "irc_network.h"
#include "config.h"
#include "server.h"
#include "irc.h"

#ifdef HAVE_ICS
#include "ics_server.h"
#endif /* HAVE_ICS */

#ifdef HAVE_XMPP
#include "xmpp_server.h"
#endif /* HAVE_XMPP */

/* Needs more cowbell */
void die_nicely(int ret)
{
	if (g_cfg != NULL)
	{
		free_networks(g_cfg->networks);

#ifdef HAVE_ICS
		/* free_ics_servers(g_cfg->ics_servers); */
#endif /* HAVE_ICS */

#ifdef HAVE_XMPP
		free_xmpp_servers(g_cfg->xmpp_servers);
#endif /* HAVE_XMPP */

		free_tconfig(g_cfg->tcfg);

		free_dcc_sessions(g_cfg->dccs);

#ifdef HAVE_JS
		if (g_cfg->js_rt != NULL)
			JS_DestroyRuntime(g_cfg->js_rt);
#endif


		free(g_cfg->hash_type);
		free(g_cfg);
	}

	exit(ret);
}
