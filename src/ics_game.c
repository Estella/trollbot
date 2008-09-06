#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include "main.h"

#include "ics_server.h"
#include "ics_proto.h"
#include "ics_game.h"
#include "log_entry.h"
#include "egg_lib.h"

#include "server.h"

/* Adds an ICS game to the game list */
struct ics_game *ics_game_add(struct ics_game *ics_games, struct ics_game *add)
{
	struct ics_game *tmp = NULL;

	if ((tmp = ics_games) == NULL)
		return add;

	while (tmp->next != NULL) tmp = tmp->next;

	tmp->next = add;
	add->prev = tmp;

	return ics_games;
}

/* Removes an ICS game from the game list */
struct ics_game *ics_game_del(struct ics_game *ics_games, struct ics_game *del)
{
	struct ics_game *tmp = NULL;

	if ((tmp = ics_games) == NULL)
	{
		log_entry_printf(NULL,NULL,"X","ics_game_del() called with NULL game list");
		return NULL;
	}

	while (tmp != NULL)
	{
		if (tmp == del)
		{
			if (tmp->prev != NULL)
				tmp->prev->next = tmp->next;

			if (tmp->next != NULL)
				tmp->next->prev = tmp->prev;

			while (tmp == del && tmp->prev != NULL)
				tmp = tmp->prev;

			while (tmp == del && tmp->next != NULL)
				tmp = tmp->next;

			if (tmp == del)
				return NULL;
			else
				return tmp;

		}

		tmp = tmp->next;
	}

	log_entry_printf(NULL,NULL,"X","ics_game_del() called with a game deletion that no entry existed for");

	return ics_games;
}

/* constructor */
struct ics_game *new_ics_game(void)
{
	struct ics_game *ret;
	int i;
	int j;

	ret = tmalloc(sizeof(struct ics_game));

	ret->type = ICS_GAME_UNHANDLED;

	ret->turn             = '-';
  ret->white_name       = NULL;
  ret->black_name       = NULL;
  ret->verbose_notation = NULL;
  ret->last_move_time   = NULL;
  ret->pretty_notation  = NULL; 

	ret->pawn_double_push_file = -1;
	ret->white_can_castle_short = -1;
	ret->white_can_castle_long = -1;
	ret->black_can_castle_short = -1;
	ret->black_can_castle_long = -1;
	ret->last_irreversible_move = -1;
	ret->game_number = -1;
	ret->my_relation = -1;
	ret->initial_time = -1;
	ret->increment_time = -1;
	ret->white_strength = -1;
	ret->black_strength = -1;
	ret->white_time_remaining = -1;
	ret->black_time_remaining = -1;
	ret->next_move_number = -1;
	ret->flip_field_orientation = -1;


	for (i=0; i < 8; i++)
		for (j = 0; j < 8; j++)
			ret->board[i][j] = '-';

	ret->prev     = NULL;
	ret->next     = NULL;

	return ret;
}

void free_ics_games(struct ics_game *ics_games)
{
	struct ics_game *tmp = ics_games;
	struct ics_game *old = NULL;

	if (tmp == NULL)
	{
		log_entry_printf(NULL,NULL,"X","free_ics_games() called with NULL game list");
		return;
	}

	while (tmp != NULL)
	{
		old = tmp->next;

		free_ics_game(tmp);

		tmp = old;
	}

	return;
}	

void free_ics_game(struct ics_game *ics_game)
{
	if (ics_game == NULL)
		return;

  free(ics_game->white_name);      
  free(ics_game->black_name);      
  free(ics_game->verbose_notation);
  free(ics_game->last_move_time); 
  free(ics_game->pretty_notation); 

	free(ics_game);
}
