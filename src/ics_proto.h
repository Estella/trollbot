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

struct ics_trigger;

/* Freechess */
#define LOGIN_TRIGGER "*(If your return key does not work, use cntrl-J)*"
#define PASS_TRIGGER "*If it is yours, type the password.*"
#define ENTER_TRIGGER "*Press return*"
#define CONNECT_TRIGGER "fics*"
#define MY_NAME_TRIGGER "\\*\\*\\*\\* Starting FICS session as*"
#define TELL_TRIGGER "% tells you:"
#define STYLE_TWELVE_TRIGGER "<12>*"
#define SET_GAME_TRIGGER "Game % (% vs. %)"
#define ANTI_ANTI_IDLE_TRIGGER "*unrated blitz*"
#define ANTI_ANTI_IDLE_INTERVAL 20
#define ENDGAME_TRIGGER "{Game % (% vs. %) *}*"

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
void ics_internal_connect(struct ics_server *ics, struct ics_trigger *ics_trig, struct ics_data *data);
void ics_internal_enter(struct ics_server *ics, struct ics_trigger *ics_trig, struct ics_data *data);
void init_ics_triggers(struct ics_server *ics);
void ics_internal_login(struct ics_server *ics, struct ics_trigger *ics_trig, struct ics_data *data);
void ics_ball_start_rolling(struct ics_server *ics);
void ics_printf(struct ics_server *ics, const char *fmt, ...);
struct ics_data *ics_data_new(void);
void ics_data_free(struct ics_data *data);
void parse_ics_line(struct ics_server *ics, const char *buffer);
int ics_in(struct ics_server *ics);

#endif /* __ICS_H__ */
