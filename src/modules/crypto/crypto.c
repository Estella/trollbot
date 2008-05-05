#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tomcrypt.h"

#include "util.h"

#define hash_algorithm_exists crypto_LTX_hash_algorithm_exists
#define create_hash           crypto_LTX_create_hash

int cipher_algorithm_exists(char *algo)
{
	return 0; /* Not implemented yet */
	if (!tstrcasecmp(algo,"blowfish"))
		return 1;

	return 0;
}

int hash_algoritm_exists(char *algo)
{
	/* Cheap */
	if (!tstrcasecmp(algo,"sha512"))
		return 1;

	return 0;
}

int hash_algorithm_exists(char *algo)
{
	if (!tstrcasecmp(algo,"md5"))
		return 1;

	return 0;
}

char *create_hash(char *pass, char *hash_type)
{
	hash_state md;

	/* These should be as large as the largest hash type's string/byte representation of its hash respectively */
	char *hash_string = NULL;
	unsigned char tmp[64];

	int i;
	int hash_size = 0;

	if (hash_type == NULL)
	{
		printf("Missing Hash Type argument for create_hash()\n");
		return NULL;
	}

	memset(tmp, 0, sizeof(tmp));

	if (!tstrcasecmp(hash_type,"sha512"))
	{
		sha512_init(&md);
		sha512_process(&md, (unsigned char *)pass, strlen(pass));
		sha512_done(&md, tmp);
		hash_size = 64;  /* Size in bytes */

		hash_string = tmalloc0(hash_size*2+1);
	}
	else if (!tstrcasecmp(hash_type,"md5"))
	{
		printf("Oops, started coding md5 support, but if you're reading this, I must have never finished it :/\n");
		return NULL;
	}
	else
	{
		printf("Invalid Hash type: %s\n",hash_type);
		return NULL;
	}

	/* TODO: There's probably a better way of doing this */
	for (i=0; i<hash_size; i++)
	{
		sprintf(&hash_string[strlen(hash_string)],"%0x",tmp[i]);
	}

	return hash_string;

}
