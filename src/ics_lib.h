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
#ifndef __ICS_LIB_H__
#define __ICS_LIB_H__

char *ics_get_moves_as_pgn(struct ics_server *ics, char *output);
int ics_get_score(struct ics_server *ics, char *whom);
char **ics_getboard(struct ics_server *ics, int game_id);
char **ics_bind(struct ics_server *ics, char *type, char *flags, char *mask, char *cmd, void (*handler)(struct ics_server *, struct ics_trigger *, struct ics_data *));


#endif /* __ICS_LIB_H__ */
