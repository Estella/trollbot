#ifndef __ICS_GAME_H__
#define __ICS_GAME_H__

/* ltdl stuff */
#define ics_game_add  ics_LTX_ics_game_add
#define ics_game_del  ics_LTX_ics_game_del
#define new_ics_game  ics_LTX_new_ics_game
#define free_ics_game ics_LTX_free_ics_game


enum ics_game_types
{
	ICS_GAME_UNHANDLED = 0, 
	ICS_GAME_STANDARD,
	ICS_GAME_BLITZ,
	ICS_GAME_CRAZYHOUSE,
	ICS_GAME_LIGHTNING
};

struct ics_game
{
	int type;

	/* All provided easily by style12 */
	char board[8][8];
	char turn;                  /* 'B' or 'W' */
	int pawn_double_push_file;  /* -1 if no pawn double push, otherwise the file (0-7 as a-h) */
	int white_can_castle_short; /* Can white castle short (0=no, 1=yes) */
	int white_can_castle_long;  /* Can white castle long */
	int black_can_castle_short; /* Can black castle short */
	int black_can_castle_long;  /* Can black castle long */
	int last_irreversible_move; /* the number of moves made since the last irreversible move.  (0 if last move
                                 was irreversible.  If the value is >= 100, the game can be declared a draw
                                 due to the 50 move rule.) */
	int game_number;            /* Game Number */
	char *white_name;           /* White's name */
	char *black_name;           /* Black's name */
	int my_relation;            /* my relation to this game:
                                 -3 isolated position, such as for "ref 3" or the "sposition" command
                                 -2 I am observing game being examined
                                  2 I am the examiner of this game
                                 -1 I am playing, it is my opponent's move
                                  1 I am playing and it is my move
                                  0 I am observing a game being played */
	int initial_time;           /* Initial time in seconds for the match */
	int increment_time;         /* Increment in seconds of the match */
	int white_strength;         /* White's material strength */
	int black_strength;         /* Black's material strength */
	int white_time_remaining;   /* White's remaining time in seconds */
	int black_time_remaining;   /* Black's remaining time in seconds */
	int next_move_number;       /* The current move number */
	char *verbose_notation;     /* verbose coordinate notation for the previous move ("none" if there were none) */
	char *last_move_time;       /* The time it took to make the last move in (min:sec) format (Unlike everything else) */
	char *pretty_notation;      /* The "pretty notation of the previous move ("none if there were none) */
	int flip_field_orientation; /* 1 = black at the bottom, 0 = White at the bottom */

	char *style_twelve; /* Style 12, for script handling */

	char *winner_name;
	char *loser_name;

	char *end_result; /* Can be checkmate, stalemate, adjourn, draw, abortion, forfeiture */
	char *end_message; /* Full end message, non-null on end game */

  struct ics_game *prev;
  struct ics_game *next;  
};

/* This really needs wrapped in a generic interface, how about
 * the slist/dlist shit in util.c?
 */
struct ics_game *ics_game_add(struct ics_game *ics_games, struct ics_game *add);
struct ics_game *ics_game_del(struct ics_game *ics_games, struct ics_game *del);

struct ics_game *new_ics_game(void);

void free_ics_games(struct ics_game *ics_games);
void free_ics_game(struct ics_game *ics_game);

#endif /* __ICS_GAME_H__ */
