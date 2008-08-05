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

struct ics_data
{
	char *txt_packet;
};

void ics_ball_start_rolling(struct ics_server *ics);
void ics_printf(struct ics_server *ics, const char *fmt, ...);
struct ics_data *ics_data_new(void);
void ics_data_free(struct ics_data *data);
void parse_ics_line(struct ics_server *ics, const char *buffer);
int ics_in(struct ics_server *ics);

#endif /* __ICS_H__ */
