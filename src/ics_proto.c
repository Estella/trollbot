/******************************
 * Trollbot                   *
 ******************************
 * Written by poutine DALnet  *
 ******************************
 * This software is public    *
 * domain. Free for any use   *
 * whatsoever.                *
 ******************************/
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "main.h"
#include "ics_proto.h"
#include "ics_server.h"
#include "ics_trigger.h"
#include "log_entry.h"

void init_ics_triggers(struct ics_server *ics)
{
	struct ics_trigger *trig;

	trig          = new_ics_trigger();
	trig->type    = ICS_TRIG_MSG;
	/* I know this is stupid */
	trig->mask    = tstrdup(LOGIN_TRIGGER);
	trig->handler = ics_internal_login;
	trig->command = NULL;

	ics->ics_trigger_table->msg = ics_trigger_add(ics->ics_trigger_table->msg, trig);

	trig          = new_ics_trigger();
	trig->type    = ICS_TRIG_MSG;
	/* I know this is stupid */
	trig->mask    = tstrdup(ENTER_TRIGGER);
	trig->handler = ics_internal_enter;
	trig->command = NULL;

	ics->ics_trigger_table->msg = ics_trigger_add(ics->ics_trigger_table->msg, trig);
}

void ics_internal_enter(struct ics_server *ics, struct ics_trigger *ics_trig, struct ics_data *data)
{
	ics_printf(ics, "\r\n");
}

void ics_internal_login(struct ics_server *ics, struct ics_trigger *ics_trig, struct ics_data *data)
{
	ics_printf(ics, ics->username);
}

void ics_ball_start_rolling(struct ics_server *ics)
{
	init_ics_triggers(ics);
}

/* This is different from ics_printf() (should be sock_printf())
 * in the way that it has an extra level of abstraction in the
 * case of SSL sockets.
 */
/* lol no it doesn't */
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

	printf("Sent: %s\n",buf2);
	send(ics->sock,buf2,strlen(buf2),0);
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

	free(data);
}

/* This function gets an unparsed line from ICS, and makes it into the ics_data struct */
void parse_ics_line(struct ics_server *ics, const char *buffer)
{
	struct ics_data *data    = NULL;

	data = ics_data_new();

	data->txt_packet = tstrdup(buffer);
	printf("%s\n",data->txt_packet);

	ics_trigger_match(ics, data);

	ics_data_free(data);
}

int ics_in(struct ics_server *ics)
{
	static char         *buffer  = NULL;
	static size_t       size     = BUFFER_SIZE;
	int                 recved   = 0;
	char                *line    = NULL;
	const char          *ptr     = NULL;
	char                *optr    = NULL;
	char                *bufcopy = NULL;


	if (buffer == NULL)
	{
		buffer = tmalloc0(BUFFER_SIZE + 1);
		recved = recv(ics->sock,buffer,BUFFER_SIZE-1,0);
	} else {
		/* There was a fragment left over */
		buffer = tcrealloc0(buffer,
				strlen(buffer) + BUFFER_SIZE + 1,
				&size);

		recved = recv(ics->sock,&buffer[strlen(buffer)],BUFFER_SIZE-1,0);

	}


	switch (recved)
	{
		case -1:
			free(buffer);
			buffer = NULL;
			return 1;
		case 0:
			ics->sock = -1;
			free(buffer);
			buffer = NULL;
			return 0;
	}

	while (strchr(buffer,'\n') != NULL)
	{ /* Complete IRC line */
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
