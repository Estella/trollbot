/******************************
 * Trollbot                   *
 ******************************
 * Written by poutine DALnet  *
 ******************************
 * This software is public    *
 * domain. Free for any use   *
 * whatsoever.                *
 ******************************/

#ifndef __ICS_PROTO_H__
#define __ICS_PROTO_H__

struct ics_server;
struct ics_trigger;
struct tsocket;

/* Freechess */
#define LOGIN_TRIGGER "*(If your return key does not work, use cntrl-J)*"
#define PASS_TRIGGER "*If it is yours, type the password.*"
#define ENTER_TRIGGER "*Press return*"
#define CONNECT_TRIGGER "fics*"
#define MY_NAME_TRIGGER "\\*\\*\\*\\* Starting FICS session as*"
#define STYLE_TWELVE_TRIGGER "<12>*"
#define SET_GAME_TRIGGER "Game % (% vs. %)"
#define ANTI_ANTI_IDLE_TRIGGER "*unrated blitz*"
#define ANTI_ANTI_IDLE_INTERVAL 20
#define ENDGAME_TRIGGER "{Game % (% vs. %) *}*"
#define TELL_TRIGGER "* tells you: *"

/* Things that eventually need handled 

"* kibitzes: Hello from Crafty"
"* is in the computer list."
"* * is a computer *"
"* offers to be your bughouse partner"
"* tells you: [automatic message] I chose you"
"* agrees to be your partner"
"are no longer *'s partner"
"no longer have a bughouse partner"
"partner has disconnected"
"partner has just chosen a new partner"
"* tells you: [automatic message] I'm no longer your"
"* (your partner) tells you: *"
"* tells you: *"
"* says: *"
"--> * *"
"* shouts: *"
"* kibitzes: *"
"* whispers: *"
"You have * message*."
"* has left a message for you."
"* just sent you a message."
"--* (*:*): *"
"*. * (*:*): *"
"*. * at *:*: *"
"*(*): *"
"*(*)(*): *"
"Notification: * has arrived"
"Not sent -- * is censoring you"
"command is currently turned off"
"* * match * * requested with * (*)"
"* * match * requested with * (*)"
* has made an alternate proposal of * * match * *."
"Challenge: * (*) *(*) * * * * Loaded from *"
"Challenge: * (*) *(*) * * * * : * * Loaded from *"
"Challenge: * (*) *(*) * * * * : * *"
"Challenge: * (*) *(*) * * * * * *"
"Challenge: * (*) *(*) * * * *"
"offers you a draw"
"requests that the game be aborted"
"would like to abort"
"requests adjournment"
"would like to adjourn"

backend.c:
"ics%"			* right after login only; nonessential *
"chessclub.com"         * before login; turns on ICC mode *
"\"*\" is *a registered name"
"Logging you in as \"*\""
"Your name will be \"*\""
"* s-shouts: "
"* c-shouts: "
"--->"			* seen in FICS login, not a shout *
"* shouts: "
"--> "
"* tells you: "
"* (your partner) tells you: "
"* says: "
"* has left a message "
"* just sent you a message:\n"
"*. * (*:*): "
"*. * at *:*: "
"* whispers: "
"* kibitzes: "
"*)(*): *"		 * channel tell or allobs or admin comment *
"*(*): *"		 * channel tell or allobs or admin comment *
"*)(*)(*): *"		 * channel tell *
"Challenge:"
"* offers you"
"* offers to be"
"* would like to"
"* requests to"
"Your opponent offers"
"Your opponent requests"
"\\   "                  * continuation line *
"Black Strength :"       * need to issue "style 12; refresh" *
"<<< style 10 board >>>" * need to issue "style 12; refresh" *
"<10>"                   * need to issue "style 12; refresh" *
"#@#"                    * need to issue "style 12; refresh" *
"login:"
"\n<12> "
"<12> "
"\n<b1> "
"<b1> "
"* *vs. * *--- *"        * move list coming *
"* * match, initial time: * minute*, increment: * second"
"Move  "                 * move list is here *
"% "			 * end of prompt; nonessential *
"}*"			 * ends a move list *
"Adding game * to observation list"
"Game notification: * (*) vs. * (*)"
"Entering examine mode for game *"
"has made you an examiner of game *"
"Illegal move"
"Not a legal move"
"Your king is in check"
"It isn't your turn"
"It is not your move"
"still have time"
"not out of time"
"either player is out of time"
"has timeseal; checking"
"added * seconds to"
"seconds were added to"
"clock paused"
"clock resumed"
"Creating: * (*)* * (*)"
"Creating: * (*) [*] * (*)"
"{Game * (* vs. *) *}*"
"Removing game * from observation"
"no longer observing game *"
"Game * (*) has no examiners"
"no longer examining game *"
"\n"
"*% "			 * end of prompt; nonessential *

*/

/*
 * SEEK is
 * GuestZWCN (++++) seeking 3 0 unrated blitz ("play 50" to respond)
 *
 * Tell is : FrankyDChicken(U) tells you: hi
 * Confirmation of tell is: (told FrankyDChicken)
 * Signin nickname assignment is: **** Starting FICS session as FrankyDChicken(U) ****
 * Successful custom guest is: "FrankyDChicken" is not a registered name.  You may use this name to play unrated games.
 */
struct ics_data
{
	char *txt_packet;
	char **tokens;
};

void ics_internal_tell(struct ics_server *ics, struct ics_trigger *ics_trig, struct ics_data *data);
void ics_internal_msg_handler(struct ics_server *ics, struct ics_trigger *ics_trig, struct ics_data *data);
void ics_internal_endgame(struct ics_server *ics, struct ics_trigger *ics_trig, struct ics_data *data);
void ics_internal_announce_new_game(struct ics_server *ics, struct ics_trigger *ics_trig, struct ics_data *data);
void ics_internal_call_game_triggers(struct ics_server *ics, struct ics_data *data);
void ics_internal_board_get_info(struct ics_server *ics, struct ics_trigger *ics_trig, struct ics_data *data);
void ics_internal_set_game(struct ics_server *ics, struct ics_trigger *ics_trig, struct ics_data *data);
void ics_internal_my_name(struct ics_server *ics, struct ics_trigger *ics_trig, struct ics_data *data);
void ics_internal_anti_anti_idle(struct ics_server *ics, struct ics_trigger *ics_trig, struct ics_data *data);
void ics_internal_notify(struct ics_server *ics, struct ics_trigger *ics_trig, struct ics_data *data);
void ics_internal_style_twelve_init(struct ics_server *ics, struct ics_trigger *ics_trig, struct ics_data *data);
int ics_disconnected(struct tsocket *tsock);
void ics_internal_connect(struct ics_server *ics, struct ics_trigger *ics_trig, struct ics_data *data);
void ics_internal_enter(struct ics_server *ics, struct ics_trigger *ics_trig, struct ics_data *data);
void init_ics_triggers(struct ics_server *ics);
void ics_internal_login(struct ics_server *ics, struct ics_trigger *ics_trig, struct ics_data *data);
int ics_ball_start_rolling(struct tsocket *tsock);

void ics_printf(struct ics_server *ics, const char *fmt, ...);
struct ics_data *ics_data_new(void);
void ics_data_free(struct ics_data *data);
void parse_ics_line(struct ics_server *ics, char *buffer);
int ics_in(struct tsocket *tsock);

void ics_printf(struct ics_server *ics, const char *fmt, ...);


#endif /* __ICS_PROTO_H__ */
