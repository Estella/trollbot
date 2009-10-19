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
#include <stdarg.h>

#include "trollbot.h"
#include "ics_proto.h"
#include "ics_server.h"
#include "ics_trigger.h"
#include "log_entry.h"
#include "ics_game.h"

#include "troll_lib.h"
#include "egg_lib.h"
#include "irc_network.h"
#include "server.h"
#include "irc_channel.h"
#include "user.h"
#include "irc.h"
#include "dcc.h"
#include "irc_trigger.h"
#include "t_crypto_module.h"
#include "t_timer.h"
#include "util.h"
#include "tsocket.h"

/* These are builtin triggers for ICS,
 * They can be overriden in languages
 * but I'm not sure if I've worked that
 * out or not.
 */
void init_ics_triggers(struct ics_server *ics)
{
	struct ics_trigger *trig;

	/* Logs in */
	trig          = new_ics_trigger();
	trig->type    = ICS_TRIG_MSG;
	trig->mask    = tstrdup(LOGIN_TRIGGER);
	trig->handler = ics_internal_login;
	trig->command = NULL;
	ics->ics_trigger_table->msg = ics_trigger_add(ics->ics_trigger_table->msg, trig);

	/* Generic MSG handler 
	trig          = new_ics_trigger();
	trig->type    = ICS_TRIG_MSG;
	trig->mask    = tstrdup("*");
	trig->handler = ics_internal_msg_handler;
	trig->command = NULL;
	ics->ics_trigger_table->msg = ics_trigger_add(ics->ics_trigger_table->msg, trig);*/

	/* Hits return when ICS asks it to do so */
	trig          = new_ics_trigger();
	trig->type    = ICS_TRIG_MSG;
	trig->mask    = tstrdup(ENTER_TRIGGER);
	trig->handler = ics_internal_enter;
	trig->command = NULL;
	ics->ics_trigger_table->msg = ics_trigger_add(ics->ics_trigger_table->msg, trig);

	/* Anti Anti Idle (should be a t_timer) */
	trig          = new_ics_trigger();
	trig->type    = ICS_TRIG_MSG;
	trig->mask    = tstrdup(ANTI_ANTI_IDLE_TRIGGER);
	trig->handler = ics_internal_anti_anti_idle;
	trig->command = NULL;
	ics->ics_trigger_table->msg = ics_trigger_add(ics->ics_trigger_table->msg, trig);

	/* For the on connect event trigger handling, catch the first fics% */
	trig          = new_ics_trigger();
	trig->type    = ICS_TRIG_MSG;
	trig->mask    = tstrdup(CONNECT_TRIGGER);
	trig->handler = ics_internal_connect;
	trig->command = NULL;
	ics->ics_trigger_table->msg = ics_trigger_add(ics->ics_trigger_table->msg, trig);

	/* for calling on tell events */
	trig          = new_ics_trigger();
	trig->type    = ICS_TRIG_MSG;
	trig->mask    = tstrdup(TELL_TRIGGER);
	trig->handler = ics_internal_tell;
	trig->command = NULL;
	ics->ics_trigger_table->msg = ics_trigger_add(ics->ics_trigger_table->msg, trig);

	/* For the on connect event trigger handling, call all ON CONNECT triggers */
	trig          = new_ics_trigger();
	trig->type    = ICS_TRIG_CONNECT;
	trig->mask    = tstrdup(CONNECT_TRIGGER);
	trig->handler = ics_internal_style_twelve_init;
	trig->command = NULL;
	ics->ics_trigger_table->connect = ics_trigger_add(ics->ics_trigger_table->connect, trig);

	/* For the notify */
	trig          = new_ics_trigger();
	trig->type    = ICS_TRIG_MSG;
	trig->mask    = tstrdup("Notification:*");
	trig->handler = ics_internal_notify;
	trig->command = NULL;
	ics->ics_trigger_table->msg = ics_trigger_add(ics->ics_trigger_table->msg, trig);

	/* For handling what the server calls me */
	trig          = new_ics_trigger();
	trig->type    = ICS_TRIG_MSG;
	trig->mask    = tstrdup(MY_NAME_TRIGGER);
	trig->handler = ics_internal_my_name;
	trig->command = NULL;
	ics->ics_trigger_table->msg = ics_trigger_add(ics->ics_trigger_table->msg, trig);

	/* Handling updates of the board */
	trig          = new_ics_trigger();
	trig->type    = ICS_TRIG_MSG;
	trig->mask    = tstrdup(STYLE_TWELVE_TRIGGER);
	trig->handler = ics_move_or_new_game;
	trig->command = NULL;
	ics->ics_trigger_table->msg = ics_trigger_add(ics->ics_trigger_table->msg, trig);

	/* Announcing game on IRC *
	trig          = new_ics_trigger();
	trig->type    = ICS_TRIG_GAME;
	trig->mask    = NULL;
	trig->handler = ics_internal_announce_new_game;
	trig->command = NULL;
	ics->ics_trigger_table->game = ics_trigger_add(ics->ics_trigger_table->game, trig);
*/

	/* Handling updates of the board */
	trig          = new_ics_trigger();
	trig->type    = ICS_TRIG_MSG;
	trig->mask    = tstrdup(ENDGAME_TRIGGER);
	trig->handler = ics_internal_endgame;
	trig->command = NULL;
	ics->ics_trigger_table->msg = ics_trigger_add(ics->ics_trigger_table->msg, trig);


}

void ics_internal_tell(struct ics_server *ics, struct ics_trigger *ics_trig, struct ics_data *data)
{
	struct ics_trigger *trig;
	/* char *mask; */
	
	trig = ics->ics_trigger_table->tell;

	while (trig != NULL)
	{
		if (!matchwilds(data->txt_packet, trig->mask))
		{
			if (trig->handler != NULL)
			{
				trig->handler(ics, trig, data);
				trig->usecount++;
			}
		}
			
		trig = trig->next;
	}

	return;
}

void ics_internal_endgame(struct ics_server *ics, struct ics_trigger *ics_trig, struct ics_data *data)
{
	struct ics_trigger *trig;
	int argc = tstrcount(data->tokens);

	/* We've received a sign of the end */
	/* fill in information required by scripting */
	
	if (!strcmp(data->tokens[argc-1], "*"))
	{
		/* This means it's an abortion, adjournment, TODO: verify what else */

		ics->game->winner_name = tstrdup(ics->game->white_name);
		ics->game->loser_name  = tstrdup(ics->game->black_name);
	} 
	else if (strstr(data->txt_packet, "1/2-1/2") != NULL) 
	{
		/* This means it's a stalemate, draw */
		if (!strncmp(data->tokens[argc-2], "stalemate", 9))
			ics->game->end_result = tstrdup("stalemate");
		else
			ics->game->end_result = tstrdup("draw");

		ics->game->winner_name = tstrdup(ics->game->white_name);
		ics->game->loser_name  = tstrdup(ics->game->black_name);
	}
	else if (strstr(data->txt_packet, "1-0") != NULL)
	{
		/* White won */
		if (!strncmp(data->tokens[argc-2], "checkmated", 10))
			ics->game->end_result = tstrdup("checkmate");
		else if (!strncmp(data->tokens[argc-2], "resigns", 7))
			ics->game->end_result = tstrdup("resignation");
		else if (!strncmp(data->tokens[1], "forfeits", 8))
			ics->game->end_result = tstrdup("forfeiture");

		ics->game->winner_name = tstrdup(ics->game->white_name);
		ics->game->loser_name  = tstrdup(ics->game->black_name);
	}
	else if (strstr(data->txt_packet, "0-1") != NULL)
	{
		/* Black won */
		if (!strncmp(data->tokens[argc-2], "checkmated", 10))
			ics->game->end_result = tstrdup("checkmate");
		else if (!strncmp(data->tokens[argc-2], "resigns", 7))
			ics->game->end_result = tstrdup("resignation");
		else if (!strncmp(data->tokens[1], "forfeits", 8))
			ics->game->end_result = tstrdup("forfeiture");

		ics->game->winner_name = tstrdup(ics->game->black_name);
		ics->game->loser_name  = tstrdup(ics->game->white_name);
	}

	if (!strncmp(data->tokens[argc-2], "aborted", 7))
		ics->game->end_result = tstrdup("abort");

	if (ics->game->end_result == NULL)
		ics->game->end_result  = tstrdup("WIN");

	ics->game->end_message = tstrdup(data->txt_packet);

	trig = ics->ics_trigger_table->endgame;

	while (trig != NULL)
	{
		if (trig->handler != NULL)
		{
			trig->handler(ics, trig, data);
			trig->usecount++;
		}
			
		trig = trig->next;
	}

	free_ics_games(ics->game);
	ics->game = NULL;

	return;
}

void ics_internal_msg_handler(struct ics_server *ics, struct ics_trigger *ics_trig, struct ics_data *data)
{
	struct ics_trigger *trig;
	
	trig = ics->ics_trigger_table->msg;

	while (trig != NULL)
	{
		if (!matchwilds(data->txt_packet, trig->mask) && trig != ics_trig)
		{
			if (trig->handler != NULL)
			{
				trig->handler(ics, trig, data);
				trig->usecount++;
			}
		}
			
		trig = trig->next;
	}

	return;
}


void ics_move_or_new_game(struct ics_server *ics, struct ics_trigger *ics_trig, struct ics_data *data)
{
	struct ics_trigger *trig;

	ics_internal_board_get_info(ics, ics_trig, data);
	printf("We have move\n");
	
	trig = ics->ics_trigger_table->move;

	while (trig != NULL)
	{
		if (trig->handler != NULL)
		{
			printf("Found trigger\n");
			trig->handler(ics, trig, data);
			trig->usecount++;
		}
			
		trig = trig->next;
	}

	return;
}

void ics_internal_board_get_info(struct ics_server *ics, struct ics_trigger *ics_trig, struct ics_data *data)
{
	int i;
	struct ics_game *game = NULL;

	game      = new_ics_game();

	/* Style 12 makes this VERY simple, thanks to siggemannen for the recommendation */
	/* Token 0 : <12> is the very beginning
	 * Token 1 : 8th rank (Black's 1st/base rank, 8 letters, - for unoccupied
	 * Token 2 : 7th rank (Black's 2nd      rank, 8 letters, - for unoccupied
	 * Token 3 : 6th rank (Black's 3rd      rank, 8 letters, - for unoccupied
	 * Token 4 : 5th rank (Black's 4th      rank, 8 letters, - for unoccupied
	 * Token 5 : 4th rank (Black's 5th      rank, 8 letters, - for unoccupied
	 * Token 6 : 3rd rank (Black's 6th      rank, 8 letters, - for unoccupied
	 * Token 7 : 2nd rank (Black's 7th      rank, 8 letters, - for unoccupied
	 * Token 8 : 1st rank (Black's 8th      rank, 8 letters, - for unoccupied
	 * Token 9 : color whose turn it is to move ("B" or "W")
	 * Token 10: -1 if the previous move was NOT a double pawn push, otherwise the chess 
	 *   board file  (numbered 0--7 for a--h) in which the double push was made
	 *
	 * Token 11: can White still castle short (0=no, 1=yes)
	 * Token 12: can White still castle long?
	 * Token 13: can Black still castle short?
	 * Token 14: can Black still castle long?
	 * Token 15: the number of moves made since the last irreversible move.  (0 if last move
	 *   was irreversible.  If the value is >= 100, the game can be declared a draw
	 *   due to the 50 move rule.)
	 * Token 16: The game number
	 * Token 17: White's name
	 * Token 18: Black's name
	 * Token 19: my relation to this game:
	 *   -3 isolated position, such as for "ref 3" or the "sposition" command
	 *   -2 I am observing game being examined
	 *    2 I am the examiner of this game
	 *   -1 I am playing, it is my opponent's move
	 *    1 I am playing and it is my move
	 *    0 I am observing a game being played
	 * Token 20: initial time (in seconds) of the match
	 * Token 21: increment In seconds) of the match
	 * Token 22: White material strength
	 * Token 23: Black material strength
	 * Token 24: White's remaining time
	 * Token 25: Black's remaining time
	 * Token 26: the number of the move about to be made (standard chess numbering -- White's
	 *   and Black's first moves are both 1, etc.)
	 * Token 27: verbose coordinate notation for the previous move ("none" if there were
	 *   none) [note this used to be broken for examined games]
	 * Token 28: time taken to make previous move "(min:sec)".
	 * Token 29: pretty notation for the previous move ("none" if there is none)
	 * Token 30: flip field for board orientation: 1 = Black at bottom, 0 = White at bottom.
	 */
	for (i = 0; i < 8; i++)
	{
		strncpy(game->board[i], data->tokens[i+1], 8);
	}

	game->turn = data->tokens[9][0];

	/* These need updated, perhaps event types for handlers */
	game->pawn_double_push_file = atoi(data->tokens[10]);
	game->white_can_castle_short = atoi(data->tokens[11]);
	game->white_can_castle_long = atoi(data->tokens[12]);
	game->black_can_castle_short = atoi(data->tokens[13]);
	game->black_can_castle_long = atoi(data->tokens[14]);
	game->last_irreversible_move = atoi(data->tokens[15]);
	game->game_number = atoi(data->tokens[16]);

	game->white_name = tstrdup(data->tokens[17]);
	game->black_name = tstrdup(data->tokens[18]);

	game->my_relation = atoi(data->tokens[19]);
	game->initial_time = atoi(data->tokens[20]);
	game->increment_time = atoi(data->tokens[21]);
	game->white_strength = atoi(data->tokens[22]);
	game->black_strength = atoi(data->tokens[23]);
	game->white_time_remaining = atoi(data->tokens[24]);
	game->black_time_remaining = atoi(data->tokens[25]);
	game->next_move_number = atoi(data->tokens[26]);

	/* These all need updated, and the event handlers called, I should use static memory */
	game->verbose_notation = tstrdup(data->tokens[27]);
	game->last_move_time   = tstrdup(data->tokens[28]);
	game->pretty_notation  = tstrdup(data->tokens[29]);

	game->flip_field_orientation = atoi(data->tokens[30]);

	game->style_twelve = tstrdup(data->txt_packet);

	if (ics->game == NULL)
	{
		ics->game = game;

		/* Start of a new game, or just joining into view it */
		ics_internal_call_game_triggers(ics, data);
	}
	else
		ics->game = game;		


	return;
}

void ics_internal_call_game_triggers(struct ics_server *ics, struct ics_data *data)
{
	struct ics_trigger *trig;
	
	trig = ics->ics_trigger_table->game;

	while (trig != NULL)
	{
		if (trig->handler != NULL)
		{
			trig->handler(ics, trig, data);
			trig->usecount++;
		}
			
		trig = trig->next;
	}

	return;
}

void ics_internal_set_game(struct ics_server *ics, struct ics_trigger *ics_trig, struct ics_data *data)
{
	
}

void ics_internal_my_name(struct ics_server *ics, struct ics_trigger *ics_trig, struct ics_data *data)
{
	char *my_name;
	size_t my_name_len;

	for (my_name_len=0; data->tokens[5][my_name_len] != '\0' && data->tokens[5][my_name_len] != '('; my_name_len++);

	my_name = tmalloc0(my_name_len + 1);
	
	strncpy(my_name, data->tokens[5], my_name_len);

	ics->my_name = my_name;

	troll_debug(LOG_DEBUG, "My name on ICS server (%s) is %s", ics->label, my_name);
}

/* Something better needs worked out for this */
void ics_internal_anti_anti_idle(struct ics_server *ics, struct ics_trigger *ics_trig, struct ics_data *data)
{
	/* struct ics_trigger *trig; */
	static int interval = ANTI_ANTI_IDLE_INTERVAL;

	interval--;

	if (interval == 0)
	{
		interval = ANTI_ANTI_IDLE_INTERVAL;
		ics_printf(ics, "games");
	}

	return;
}


void ics_internal_notify(struct ics_server *ics, struct ics_trigger *ics_trig, struct ics_data *data)
{
	struct ics_trigger *trig;

	trig = ics->ics_trigger_table->notify;

	/* Call all triggers that match the username of whoever was on your notify list */
	while (trig != NULL)
	{
		if (!matchwilds(data->tokens[1],trig->mask))
		{
			if (trig->handler != NULL)
			{
				trig->handler(ics, trig, data);
			}
		}

		trig = trig->next;
	}
	
	return;
}

void ics_internal_style_twelve_init(struct ics_server *ics, struct ics_trigger *ics_trig, struct ics_data *data)
{
	/* struct ics_trigger *trig; */

	ics_printf(ics, "style 12");
	ics_printf(ics, "set shout 0");
	ics_printf(ics, "set cshout 0");
	ics_printf(ics, "set open 1");
	ics_printf(ics, "set highlight 0");
	ics_printf(ics, "set bell 0");
	ics_printf(ics, "set interface Trollbot, by poutine/DALnet");

	return;
}

void ics_internal_connect(struct ics_server *ics, struct ics_trigger *ics_trig, struct ics_data *data)
{
	struct ics_trigger *trig;

	if (ics->connected != 0)
		return;

	trig = ics->ics_trigger_table->connect;

	while (trig != NULL)
	{
		if (trig->handler != NULL)
		{
			trig->handler(ics, trig, data);
		}

		trig = trig->next;
	}

	ics->connected = 1;

	return;
}

/* This sents a carriage return and newline when the server prompts you to hit enter */
void ics_internal_enter(struct ics_server *ics, struct ics_trigger *ics_trig, struct ics_data *data)
{
	ics_printf(ics, "\r\n");
}

/* This simply sends the username in the conf file when prompted for it */
void ics_internal_login(struct ics_server *ics, struct ics_trigger *ics_trig, struct ics_data *data)
{
	ics_printf(ics, ics->username);
}

int ics_disconnected(struct tsocket *tsock)
{
	struct ics_server *ics = tsock->data;

	tsocket_close(tsock);
	
	ics_server_connect(ics, tsock);

	return 1;
}

/* This is called on connect? */
int ics_ball_start_rolling(struct tsocket *tsock)
{
	struct ics_server *ics = tsock->data;
	init_ics_triggers(ics);

	return 1;
}

/* This is like irc_printf but it takes an ics_server and not a socket,
 * so this can be abstracted later.
 */
void ics_printf(struct ics_server *ics, const char *fmt, ...)
{
	va_list va;
	char buf[2048];
	char buf2[2059];

	memset(buf, 0, sizeof(buf));
	memset(buf2, 0, sizeof(buf2));

	va_start(va, fmt);

	/* C99 */
	vsnprintf(buf, sizeof(buf), fmt, va);
	va_end(va);

	snprintf(buf2,sizeof(buf2),"%s\n",buf);

/*	printf("Sent: %s\n",buf2);*/
	send(ics->tsock->sock,buf2,strlen(buf2),0);
}

/* Constructor */
struct ics_data *ics_data_new(void)
{
	struct ics_data *local;

	local = tmalloc(sizeof(struct ics_data));

	local->txt_packet = NULL;

	return local;
}

/* Destructor */
void ics_data_free(struct ics_data *data)
{
	free(data->txt_packet);
	tstrfreev(data->tokens);

	free(data);
}

/* This function gets an unparsed line from ICS, and makes it into the ics_data struct */
void parse_ics_line(struct ics_server *ics, char *buffer)
{
	struct ics_data *data    = NULL;

	if (strlen(buffer) == 0)
		return;

	data = ics_data_new();

	data->txt_packet = tstrdup(buffer);
	data->tokens     = tssv_split(buffer);

	/* Should be log_entry with letter 'I' like 'X' for XMPP */
	troll_debug(LOG_DEBUG, "%s\n",data->txt_packet);

	ics_trigger_match(ics, data);

	ics_data_free(data);
}

int ics_in(struct tsocket *tsock)
{
	static char         *buffer  = NULL;
	static size_t       size     = BUFFER_SIZE;
	int                 recved   = 0;
	char                *line    = NULL;
	const char          *ptr     = NULL;
	char                *optr    = NULL;
	char                *bufcopy = NULL;
	struct ics_server   *ics     = tsock->data;


	/* The previous line never existed, or it was completed and
	 * set to NULL upon completion.
	 */
	if (buffer == NULL)
	{
		/* Start with a new zeroed buffer */
		buffer = tmalloc0(BUFFER_SIZE + 1);
		recved = recv(ics->tsock->sock,buffer,BUFFER_SIZE-1,0);
	} else {
		/* There was a fragment left over, create a larger buffer */
		buffer = tcrealloc0(buffer,
				strlen(buffer) + BUFFER_SIZE + 1,
				&size);

		recved = recv(ics->tsock->sock,&buffer[strlen(buffer)],BUFFER_SIZE-1,0);

	}

	/* On Errors, or socket close */
	switch (recved)
	{
		case -1:
			free(buffer);
			buffer = NULL;
			return 1;
		case 0:
			ics->tsock->sock = -1;
			free(buffer);
			buffer = NULL;
			return 0;
	}

	while (strchr(buffer,'\n') != NULL)
	{ /* Complete ICS line */
		line = tmalloc0(strlen(buffer)+1);

		optr = line;

		for(ptr = buffer;*ptr != '\n' && *ptr != '\r';ptr++)
		{
			*optr = *ptr;
			optr++;
		}

		/* This should deal with icsds which output \r only, \r\n, or \n */
		while (*ptr == '\r' || *ptr == '\n')
			ptr++;

		/* Pass the single line for more processing */
		parse_ics_line(ics,line);

		free(line);
		line = NULL;

		if (strlen(ptr) == 0)
		{
			free(buffer);
			buffer = NULL;
			break;
		}

		bufcopy = tstrdup(ptr);

		free(buffer);

		size   = strlen(bufcopy) + 1;

		buffer = bufcopy;
	}

	return 1;

}
