#include "main.h"
#include "network.h"
#include "tconfig.h"
#include "user.h"
#include "log_entry.h"

struct tconfig_block *users_to_tconfig(struct user *users)
{
	struct user          *tmp  = NULL;
	struct tconfig_block *tcfg = NULL;
	struct tconfig_block *tpar = NULL;

	tmp = users;

	while (tmp != NULL)
	{
		if (tcfg == NULL)
			tcfg = tconfig_block_new();
		else
		{
			tcfg->next       = tconfig_block_new();
			tcfg->next->prev = tcfg;
			tcfg             = tcfg->next;
		}

		/* Should work out some memory logic to not have to do this with keys */
		tcfg->key   = tstrdup("user");
		tcfg->value = tstrdup(tmp->username);

		/* Ensure at least one entry exists so we don't have keys with all nulled keys/values */
		if (tmp->nick == NULL && tmp->ident == NULL && tmp->host == NULL && 
				tmp->uhost == NULL && tmp->passhash == NULL && tmp->hash_type == NULL &&
				tmp->flags == NULL)
		{
			tmp = tmp->next;
			continue;
		}

		/* Create child, save parent pointer */
		tcfg->child  = tconfig_block_new();
		tpar         = tcfg;
		tcfg         = tcfg->child;
		tcfg->parent = tpar;

		if (tmp->nick != NULL)
		{
			/* Nick */
			tcfg->key        = tstrdup("nick");
			tcfg->value      = tstrdup(tmp->nick);
			tcfg->next       = tconfig_block_new();
			tcfg->next->prev = tcfg;
			tcfg             = tcfg->next;
		}

		if (tmp->ident != NULL)
		{
			/* Ident */
			tcfg->key        = tstrdup("ident");
			tcfg->value      = tstrdup(tmp->ident);
			tcfg->next       = tconfig_block_new();
			tcfg->next->prev = tcfg;
			tcfg             = tcfg->next;
		}

		if (tmp->host != NULL)
		{
			/* Host */
			tcfg->key        = tstrdup("host");
			tcfg->value      = tstrdup(tmp->host);
			tcfg->next       = tconfig_block_new();
			tcfg->next->prev = tcfg;
			tcfg             = tcfg->next;
		}

		if (tmp->uhost != NULL)
		{
			/* UHost */
			tcfg->key        = tstrdup("uhost");
			tcfg->value      = tstrdup(tmp->uhost);
			tcfg->next       = tconfig_block_new();
			tcfg->next->prev = tcfg;
			tcfg             = tcfg->next;
		}

		if (tmp->passhash != NULL)
		{
			/* Passhash */
			tcfg->key        = tstrdup("passhash");
			tcfg->value      = tstrdup(tmp->passhash);
			tcfg->next       = tconfig_block_new();
			tcfg->next->prev = tcfg;
			tcfg             = tcfg->next;
		}

		if (tmp->hash_type != NULL)
		{
			/* Hash type */
			tcfg->key        = tstrdup("hash_type");
			tcfg->value      = tstrdup(tmp->hash_type);
			tcfg->next       = tconfig_block_new();
			tcfg->next->prev = tcfg;
			tcfg             = tcfg->next;
		}

		if (tmp->flags != NULL)
		{
			/* flags */
			tcfg->key   = tstrdup("flags");
			tcfg->value = tstrdup(tmp->flags);
		}

		/* Switch to Parent pointer */
		tcfg = tpar;

		tmp  = tmp->next;
	}

	if (tcfg != NULL)
		while (tcfg->prev != NULL)
			tcfg = tcfg->prev;

	/* To be returned by caller */
	return tcfg;
}




void user_list_add(struct user **orig, struct user *new)
{
	struct user *tmp;

	if (*orig == NULL)
	{
		*orig = new;
		new->prev = NULL;
		new->next = NULL;
	}
	else
	{
		tmp = *orig;

		while (tmp->next != NULL)
			tmp = tmp->next;

		tmp->next       = new;
		tmp->next->prev = tmp;
		tmp->next->next = NULL;
	}
}

void free_users(struct user *users)
{
	struct user *utmp;
	struct channel_flags *cftmp;

	if (users == NULL)
		return;

	while (users->prev != NULL)
		users = users->prev;

	while (users != NULL)
	{
		free(users->username);
		free(users->nick);
		free(users->ident);
		free(users->host);
		free(users->uhost);
		free(users->realname);
		free(users->passhash);
		free(users->hash_type);
		free(users->flags); 

		if (users->chan_flags != NULL)
		{
			while (users->chan_flags->prev != NULL)
				users->chan_flags = users->chan_flags->prev;

			while (users->chan_flags != NULL)
			{
				free(users->chan_flags->chan);
				free(users->chan_flags->flags);

				cftmp = users->chan_flags; 

				users->chan_flags = users->chan_flags->next;

				free(cftmp);
			}
		}

		utmp  = users;
		users = users->next;

		free(utmp);
	}
}


struct user *new_user(char *username, char *nick, char *passhash, char *ident, char *realname, char *host, char *flags)
{
	struct user *ret;

	ret = tmalloc(sizeof(struct user));

	ret->username = (username != NULL) ? tstrdup(username) : NULL;  
	ret->nick     = (nick != NULL)     ? tstrdup(nick)     : NULL;
	ret->passhash = (passhash != NULL) ? tstrdup(passhash) : NULL;
	ret->ident    = (ident != NULL)    ? tstrdup(ident)    : NULL;
	ret->realname = (realname != NULL) ? tstrdup(realname) : NULL;
	ret->host     = (host != NULL)     ? tstrdup(host)     : NULL;
	ret->flags    = (flags != NULL)    ? tstrdup(flags)    : NULL;

	ret->hash_type = NULL;
	ret->uhost     = NULL;

	ret->chan_flags = NULL;

	ret->tcfg   = NULL;
	ret->prev   = NULL;
	ret->next   = NULL;

	return ret;
}

struct channel_flags *new_channel_flags(char *chan, char *flags)
{
	struct channel_flags *ret;

	ret = tmalloc(sizeof(struct channel_flags));

	ret->chan  = (chan != NULL)  ? tstrdup(chan)  : NULL;
	ret->flags = (flags != NULL) ? tstrdup(flags) : NULL;

	ret->prev = NULL;
	ret->next = NULL;

	return ret;
}

/* Saves users with optional network */
void users_save(struct network *net)
{
	struct tconfig_block *tcfg   = NULL;
	char                 *tmpstr = NULL;
	struct network       *nettmp = NULL;

	log_entry_printf(net, NULL, "c", "Saving user file...");

	if (net != NULL)
	{
		tcfg = users_to_tconfig(net->users);

		if (net->userfile == NULL)
		{
			tmpstr = tmalloc0(strlen("./db/userdb.") + strlen(net->label) + 1);
			sprintf(tmpstr, "./db/userdb.%s",nettmp->label);

			tconfig_to_file(tcfg, tmpstr);

			free(tmpstr);
		}
		else
			tconfig_to_file(tcfg, net->userfile);

		free_tconfig(tcfg);
		return;
	}

	nettmp = g_cfg->networks;

	if (nettmp == NULL)
	{
		free_tconfig(tcfg);
		return;
	}

	while (nettmp->prev != NULL) nettmp = nettmp->prev;

	while (nettmp != NULL)
	{
		tcfg = users_to_tconfig(nettmp->users);

		if (nettmp->userfile == NULL)
		{
			tmpstr = tmalloc0(strlen("./db/userdb.") + strlen(nettmp->label) + 1);
			sprintf(tmpstr, "./db/userdb.%s",nettmp->label);

			tconfig_to_file(tcfg, tmpstr);

			free(tmpstr);
		}
		else
			tconfig_to_file(tcfg, nettmp->userfile);

		free_tconfig(tcfg);

		nettmp = nettmp->next;
	}

	return;
}


struct user *new_user_from_tconfig_block(struct tconfig_block *tcfg)
{
	struct user          *user  = NULL;
	struct tconfig_block *child = NULL;

	if (tcfg == NULL)
	{
		troll_debug(LOG_ERROR, "new_user_from_tconfig_block() called with NULL argument");
		return NULL;
	}

	if (!strcmp(tcfg->key,"user"))
	{
		/* New User Record */
		user = new_user(tcfg->value,NULL,NULL,NULL,NULL,NULL,NULL); /* FIXME: That's retarded */

		child = tcfg->child;

		while (child != NULL)
		{
			if (!strcmp(child->key,"nick"))
			{
				if (user->nick == NULL)
					user->nick = tstrdup(child->value);
			}
			else if (!strcmp(child->key,"passhash"))
			{
				if (user->passhash == NULL)
					user->passhash = tstrdup(child->value);
			}
			else if (!strcmp(child->key,"hash_type"))
			{
				if (user->hash_type == NULL)
					user->hash_type = tstrdup(child->value);
			}
			else if (!strcmp(child->key,"ident"))
			{
				if (user->ident == NULL)
					user->ident = tstrdup(child->value);
			}
			else if (!strcmp(child->key,"realname"))
			{
				if (user->realname == NULL)
					user->realname = tstrdup(child->value);
			}
			else if (!strcmp(child->key,"host"))
			{
				if (user->host == NULL)
					user->host = tstrdup(child->value);
			}
			else if (!strcmp(child->key,"uhost"))
			{
				if (user->uhost == NULL)
					user->uhost = tstrdup(child->value);
			}
			else if (!strcmp(child->key,"flags"))
			{
				if (user->flags == NULL)
					user->flags = tstrdup(child->value);
			}

			child = child->next;

		}
	}

	return user;
}

void user_init(void)
{
	struct network       *net      = NULL;
	struct tconfig_block *usertcfg = NULL;
	struct tconfig_block *tmp      = NULL;
	struct user          *user     = NULL;
	struct user          *tmpuser  = NULL;

	net  = g_cfg->networks;

	while (net != NULL)
	{
		if (net->userfile != NULL)
		{
			/* The idea is to read the userfile, parse the
			 * returned data into the internal format, attach
			 * location of tcfg entry, and when saved, the
			 * bot will write out this tcfg. New and deleted
			 * users will have to be mirrored in the tcfg.
			 * forget that last part.
			 */
			usertcfg = file_to_tconfig(net->userfile);

			log_entry_printf(net, NULL, "c", "Userfile loaded, unpacking...");

			tmp = usertcfg;

			while (tmp != NULL)
			{
				if (!strcmp(tmp->key,"user"))
				{
					user = new_user_from_tconfig_block(tmp);

					if (user != NULL)
					{
						user->tcfg = tmp;

						tmpuser = net->users;

						/* link it into the networks shit */
						if ((tmpuser = net->users) == NULL)
						{
							net->users = user;
							tmpuser    = user;

							tmpuser->prev = NULL;
						}
						else
						{	
							while (tmpuser->next != NULL)
								tmpuser       = tmpuser->next;

							tmpuser->next = user;
							user->prev    = tmpuser;	
						}
					}
				}

				tmp = tmp->next;
			}

			/* Why the child? */
			tconfig_merge(usertcfg, net->tcfg->child);
			free_tconfig(usertcfg);
		}


		net = net->next;
	}

	return;
}

