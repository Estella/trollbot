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

struct ics_data
{
	char *txt_packet;
	char **tokens;
};

void ics_internal_notify(struct ics_server *ics, struct ics_trigger *ics_trig, struct ics_data *data);
void ics_internal_fuck_with_sal(struct ics_server *ics, struct ics_trigger *ics_trig, struct ics_data *data);
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
