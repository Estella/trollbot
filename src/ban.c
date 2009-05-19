#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "channel.h"

#include "log_entry.h"
#include "tconfig.h"
#include "irc.h"
#include "util.h"
#include "network.h"
#include "user.h"
#include "ban.h"

struct ban *ban_new(void)
{
	struct ban *ban = tmalloc(sizeof(struct ban));

	ban->mask       = NULL;
	ban->comment    = NULL;
	ban->expiration = 0;
	ban->created    = 0;
	ban->last_time  = 0;
	ban->creator    = NULL;

	return ban;
}

void ban_free(void *data)
{
	struct ban *ban = data;

	free(ban->mask);
	free(ban->comment);
	free(ban->creator);

	return;
}
