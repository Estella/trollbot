#ifndef __T_CRYPTO_MODULE__
#define __T_CRYPTO_MODULE__

#include <ltdl.h>

struct t_crypto_module
{
	lt_dlhandle handle;

	/* Basic API so far */
	int   (*cipher_algorithm_exists)(char *);
	int   (*hash_algorithm_exists)(char *);
	char *(*create_hash)(const char *, const char *);
};

struct t_crypto_module *t_crypto_module_load(char *);

#endif /* __T_CRYPTO_MODULE__ */
