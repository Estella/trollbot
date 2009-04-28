#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util.h"
#include "ics_server.h"
#include "ics_proto.h"
#include "ics_game.h"
#include "ics_trigger.h"
#include "debug.h"

#include "ics_lib.h"

char *ics_get_moves_as_pgn(struct ics_server *ics, char *output)
{
	struct ics_game *turn;

/*
	 Oh FFS, this is what output looks like
Movelist for game 25:

GuestMKYK (UNR) vs. GuestQCFB (UNR) --- Wed Apr  1, 21:48 PDT 2009
Unrated blitz match, initial time: 5 minutes, increment: 0 seconds.

Move  GuestMKYK          GuestQCFB          
----  ----------------   ----------------   
  1.  d4      (0:00)     d5      (0:00)     
  2.  Bg5     (0:01)     Nf6     (0:06)     
  3.  e3      (0:01)     Ne4     (0:03)     
  4.  Bh4     (0:02)     g5      (0:03)     
  5.  Bg3     (0:01)     Nxg3    (0:02)     
  6.  hxg3    (0:02)     e6      (0:05)     
  7.  Bd3     (0:01)     h6      (0:03)     
  8.  Nd2     (0:01)     Bg7     (0:02)     
  9.  c3      (0:02)     Nc6     (0:01)     
 10.  f4      (0:11)     Qf6     (0:13)     
 11.  Ngf3    (0:08)     g4      (0:11)
 12.  Ne5     (0:23)     Nxe5    (0:01)
 13.  fxe5    (0:02)     Qg5     (0:01)
 14.  Qe2     (0:07)
      {Still in progress} *
*/
}
char **ics_getboard(struct ics_server *ics, int game_id)
{
	struct ics_game *turn;

	turn = ics->game;

	/* If game_id is -1, show the most current */
	if (game_id == -1)
	{
		while (turn->next != NULL)
			turn = turn->next;

		return turn->board;
	}

	/* We should retrieve the board in this case */

	return NULL;
}

int ics_get_score(struct ics_server *ics, char *whom)
{
	struct ics_game *bs = ics->game;

	if (ics->game == NULL)
	{
		troll_debug(LOG_WARN,"ics_get_score() called with no active game");
		return 0;
	}

	while (bs->next != NULL)
		bs = bs->next;

	if (tstrcasecmp(whom,"white") && tstrcasecmp(whom,"black") && tstrcasecmp(whom, ics->game->white_name) && tstrcasecmp(whom, ics->game->black_name))
	{
		troll_debug(LOG_WARN,"ics_get_score() called with non white/black or player argument");
		return 0;
	}
	
	if (!tstrcasecmp(whom,"white") || !tstrcasecmp(whom, ics->game->white_name))
		return bs->white_strength;
	else
		return bs->black_strength;
}

/*
char **ics_getchanges(struct ics_server *ics, int game_id, int s_move_id, int d_move_id)
{
}
*/
/*
 * Description: 
 * 'bind' is used to attach procedures to certain events. 
 * flags are the flags the user must have to trigger the event (if applicable). 
 * proc-name is a pointer to the procedure to call for this command.
 * If the proc-name is NULL, no binding is added. 
 * Returns: 
 * name of the command that was added, or (if proc-name is NULL), a list of the current bindings for this command
*/
/* IMP_IN: Javascript, TCL */
char **ics_bind(struct ics_server *ics, char *type, char *flags, char *mask, char *cmd, void (*handler)(struct ics_server *, struct ics_trigger *, struct ics_data *))
{
	char **returnValue=NULL;
	struct ics_trigger *trigger = NULL;
	struct ics_trigger *triggerListHead = NULL;
	int numMatches=0;

	if (handler == NULL)
	{
		/* Get a list of binds matching mask and return that list. */
		if (!tstrcasecmp("msg",type)) 
			triggerListHead = ics->ics_trigger_table->msg;
		else if (!tstrcasecmp("notify",type)) 
			triggerListHead = ics->ics_trigger_table->notify;
		else if (!tstrcasecmp("error",type))  
			triggerListHead = ics->ics_trigger_table->error; 
		else if (!tstrcasecmp("connect",type))  
			triggerListHead = ics->ics_trigger_table->connect; 
		else if (!tstrcasecmp("game",type))  
			triggerListHead = ics->ics_trigger_table->game; 
		else if (!tstrcasecmp("move",type))  
			triggerListHead = ics->ics_trigger_table->move; 
		else if (!tstrcasecmp("endgame",type))  
			triggerListHead = ics->ics_trigger_table->endgame; 
		else if (!tstrcasecmp("tell",type))  
			triggerListHead = ics->ics_trigger_table->tell; 


		trigger = triggerListHead;
		while (trigger != NULL)
		{
			if (!strcmp(trigger->mask, mask))
			{	
				return NULL;
			}

			trigger = trigger->next;
		}

		returnValue = tmalloc0(sizeof(*returnValue)*(numMatches+1));

		numMatches=0;
		while (trigger != NULL)
		{
			if (!strcmp(trigger->mask, mask))
			{
				/* ok wtf */
				returnValue[numMatches++]=tstrdup(trigger->command);
			}
			trigger = trigger->next;
		}
	}
	else 
	{
		if (!tstrcasecmp("msg",type)) 
			triggerListHead = ics->ics_trigger_table->msg;
		else if (!tstrcasecmp("notify",type)) 
			triggerListHead = ics->ics_trigger_table->notify;
		else if (!tstrcasecmp("error",type))  
			triggerListHead = ics->ics_trigger_table->error; 
		else if (!tstrcasecmp("connect",type))  
			triggerListHead = ics->ics_trigger_table->connect; 
		else if (!tstrcasecmp("game",type))  
			triggerListHead = ics->ics_trigger_table->game; 
		else if (!tstrcasecmp("move",type))  
			triggerListHead = ics->ics_trigger_table->move; 
		else if (!tstrcasecmp("endgame",type))  
			triggerListHead = ics->ics_trigger_table->endgame; 
		else if (!tstrcasecmp("tell",type))  
			triggerListHead = ics->ics_trigger_table->tell; 


		trigger = triggerListHead;
		while (trigger != NULL)
		{
			if (!strcmp(trigger->mask, mask))
			{	
				return NULL;
			}

			trigger = trigger->next;
		}

		returnValue = tmalloc0(sizeof(*returnValue));

		if (!tstrcasecmp("msg",type))
		{
			trigger          = new_ics_trigger();
			trigger->type    = ICS_TRIG_MSG;
			trigger->mask    = tstrdup(mask);
			trigger->handler = handler;
			trigger->command = tstrdup(cmd);
			ics->ics_trigger_table->msg = ics_trigger_add(ics->ics_trigger_table->msg, trigger);

			*returnValue=tstrdup(cmd);
		}
		else if (!tstrcasecmp("notify",type))
		{
			trigger          = new_ics_trigger();
			trigger->type    = ICS_TRIG_NOTIFY;
			trigger->mask    = tstrdup(mask);
			trigger->handler = handler;
			trigger->command = tstrdup(cmd);
			ics->ics_trigger_table->notify = ics_trigger_add(ics->ics_trigger_table->notify, trigger);

			*returnValue=tstrdup(cmd);
		}  
		else if (!tstrcasecmp("error",type))
		{
      trigger          = new_ics_trigger();
      trigger->type    = ICS_TRIG_ERROR;
      trigger->mask    = tstrdup(mask);
      trigger->handler = handler;
      trigger->command = tstrdup(cmd);
      ics->ics_trigger_table->error = ics_trigger_add(ics->ics_trigger_table->error, trigger);

      *returnValue=tstrdup(cmd);
		}
		else if (!tstrcasecmp("connect",type))
		{ 
      trigger          = new_ics_trigger();
      trigger->type    = ICS_TRIG_CONNECT;
      trigger->mask    = tstrdup(mask);
      trigger->handler = handler;
      trigger->command = tstrdup(cmd);
      ics->ics_trigger_table->connect = ics_trigger_add(ics->ics_trigger_table->connect, trigger);

      *returnValue=tstrdup(cmd);
		}
		else if (!tstrcasecmp("game",type))
		{ 
      trigger          = new_ics_trigger();
      trigger->type    = ICS_TRIG_GAME;
      trigger->mask    = tstrdup(mask);
      trigger->handler = handler;
      trigger->command = tstrdup(cmd);
      ics->ics_trigger_table->game = ics_trigger_add(ics->ics_trigger_table->game, trigger);

      *returnValue=tstrdup(cmd);
		}
		else if (!tstrcasecmp("move",type))
		{ 
      trigger          = new_ics_trigger();
      trigger->type    = ICS_TRIG_MOVE;
      trigger->mask    = tstrdup(mask);
      trigger->handler = handler;
      trigger->command = tstrdup(cmd);
      ics->ics_trigger_table->move = ics_trigger_add(ics->ics_trigger_table->move, trigger);

      *returnValue=tstrdup(cmd);
		}
		else if (!tstrcasecmp("endgame",type))
		{ 
      trigger          = new_ics_trigger();
      trigger->type    = ICS_TRIG_ENDGAME;
      trigger->mask    = tstrdup(mask);
      trigger->handler = handler;
      trigger->command = tstrdup(cmd);
      ics->ics_trigger_table->endgame = ics_trigger_add(ics->ics_trigger_table->endgame, trigger);

      *returnValue=tstrdup(cmd);
		}
		else if (!tstrcasecmp("tell",type))
		{ 
			if (numMatches > 0 )
				return NULL;

      trigger          = new_ics_trigger();
      trigger->type    = ICS_TRIG_TELL;
      trigger->mask    = tstrdup(mask);
      trigger->handler = handler;
      trigger->command = tstrdup(cmd);
      ics->ics_trigger_table->tell = ics_trigger_add(ics->ics_trigger_table->tell, trigger);

      *returnValue=tstrdup(cmd);
		}


	}

	return returnValue;
}
