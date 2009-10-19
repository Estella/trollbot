#include <EXTERN.h>
#include <perl.h>

#include "trollbot.h"
#include "perl_embed.h"
#include "perl_lib.h"

#include "irc_network.h"

void net_init_perl(struct network *net)
{
	net->perlinterp = perl_alloc();

	perl_construct(net->perlinterp);

	troll_debug(LOG_DEBUG,"Started perl interpreter for net (%s)",net->label);
}


