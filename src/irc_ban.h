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
#ifndef __IRC_BAN_H__
#define __IRC_BAN_H__

#include <time.h>

struct irc_ban
{
	char *mask;
	char *comment;
	time_t expiration;
	time_t created;
	time_t last_time;

	char *creator;
};

int irc_ban_evaluate(struct irc_ban *ban, char *mask);
struct irc_ban *irc_ban_new(void);
void irc_ban_free(void *data);


#endif /* __IRC_BAN_H__ */
