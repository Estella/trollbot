#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tcrypto.h"

#define tmodule_init	tcrypto_LTX_tmodule_init

int tmodule_init(void)
{
  printf("We get called\n");

  return 1;
}
