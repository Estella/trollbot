#include "main.h"
#include "perl_embed.h"
#include "perl_lib.h"

#include "network.h"

void net_init_perl(struct network *net)
{
  net->perlinterp = perl_alloc();

  perl_construct(net->perlinterp);

  printf("Perl started\n");
}


