#include "main.h"
#include "network.h"
#include "user.h"


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

void user_init(void)
{
  struct network       *net      = NULL;
	struct tconfig_block *usertcfg = NULL;
	struct tconfig_block *tmp      = NULL;
	struct tconfig_block *child    = NULL;
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
	
			tmp = usertcfg;

			while (tmp != NULL)
			{
				if (!strcmp(tmp->key,"user"))
				{
					/* New User Record */
					user = new_user(tmp->value,NULL,NULL,NULL,NULL,NULL,NULL); /* FIXME: That's retarded */

					child = tmp->child;
					
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
			
		}


		net = net->next;
	}

  return;
}

