#include "main.h"
#include "irc_trigger.h"
#include "strings.h"
#include "irc.h"
#include "network.h"
#include "server.h"
#include "channel.h"
#include "user.h"
#include "log_entry.h"

#include "egg_lib.h"
#include "troll_lib.h"

struct trigger *trigger_list_del(struct trigger *triggers, struct trigger *del)
{
	struct trigger *tmp = NULL;

	if ((tmp = triggers) == NULL)
		return NULL;

	while (tmp != NULL)
	{
		if (tmp == del)
		{
			if (tmp->prev != NULL)
				tmp->prev->next = tmp->next;

			if (tmp->next != NULL)
				tmp->next->prev = tmp->prev;

			while (tmp == del && tmp->prev != NULL)
				tmp = tmp->prev;

			while (tmp == del && tmp->next != NULL)
				tmp = tmp->next;

			if (tmp == del)
				return NULL;
			else
				return tmp;

		}

		tmp = tmp->next;
	}

	return triggers;
}

void trigger_list_add(struct trigger **orig, struct trigger *new)
{
	struct trigger *tmp;

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

struct trigger *new_trigger(char *flags, int type, char *mask, char *command, 
		void (*handler)(struct network *, struct trigger *, struct irc_data *, struct dcc_session *, const char *))
{
	struct trigger *ret;
	char *tmp;
	size_t size = 0;

	ret = tmalloc(sizeof(struct trigger));

	/* for the ease of eggdrop scripts, flags are passed as userflags|chanflags with NULL equaling - */
	if (flags != NULL)
	{
		/* If there's a user|chan pair */
		if ((tmp = strchr(flags,'|')) != NULL)
		{ 
			tmp++;
			/* tmp should now contain chanflags
			 * strncpy(dst,flags,strlen(flags) - (strlen(tmp) + 1))
			 * to get user flags
			 */
			if (tmp != NULL)
				ret->chan_flags = tstrdup(tmp);
			else
				ret->chan_flags = NULL;

			size = strlen(flags) - (strlen(tmp) + 1);

			if (size != 0)
			{
				ret->glob_flags = tmalloc0(size+1);

				strncpy(ret->glob_flags,flags,size);
			} 
			else
				ret->glob_flags = NULL;
		}
		else
		{
			ret->chan_flags = NULL;

			/* There is no |, assume all flags are global */
			if (flags[0] == '-')
				ret->glob_flags = NULL;
			else
				ret->glob_flags = tstrdup(flags);
		}
	} 
	else
	{
		ret->glob_flags = NULL;
		ret->chan_flags = NULL;
	}


	ret->type     = type;
	ret->mask     = tstrdup(mask);
	ret->command  = tstrdup(command);
	ret->handler  = handler;
	ret->usecount = 0;


	ret->prev     = NULL;
	ret->next     = NULL;

	return ret;
}

/* This is the heart of the scripting system */
void trigger_match(struct network *net, struct irc_data *data)
{
	struct trigger *trig;
	char newmask[1024];

	/* First let's determine what trigger table to check */
	if (!strcmp(data->command,"PRIVMSG"))
	{
		/* Could be PUB, PUBM, MSG, MSGM, CTCP atm */
		if (data->c_params[0] != NULL)
		{
			if (!strcasecmp(data->c_params[0],net->botnick))
			{
				/* Trigger is either MSG or MSGM */
				trig = net->trigs->msg;

				while (trig != NULL)
				{
					if (data->rest[0] != NULL)
					{
						if (!strncmp(data->rest_str,trig->mask,strlen(trig->mask)))
						{
							if (trig->handler != NULL)
							{
								trig->usecount++;
								trig->handler(net,trig,data,NULL,NULL);
							}
						}
					}

					trig = trig->next;
				}          


				trig = net->trigs->msgm;

				while (trig != NULL)
				{
					if (data->rest[0] != NULL)
					{
						if (!troll_matchwilds(data->rest_str,trig->mask))
						{
							if (trig->handler != NULL)
							{
								trig->usecount++;
								trig->handler(net,trig,data,NULL,NULL);
							}
						}

					}

					trig = trig->next;
				}

			}
			else 
			{
				/* Check bind PUB, not STACKABLE */
				trig = net->trigs->pub;

				while (trig != NULL)
				{
					if (data->rest_str == NULL)
						break; /* not likely, impossible ? */

					if (!strncmp(data->rest_str,trig->mask,strlen(trig->mask)))
					{
						if (trig->handler != NULL)
						{
							trig->usecount++;
							trig->handler(net,trig,data,NULL,NULL);
						}
						break;
					}


					trig = trig->next;
				}

				/* Check bind PUBM, STACKABLE */
				trig = net->trigs->pubm;

				while (trig != NULL)
				{
					if (data->rest[0] != NULL)
					{
						if (!troll_matchwilds(data->rest_str,trig->mask))
						{
							if (trig->handler != NULL)
							{
								trig->usecount++;
								trig->handler(net,trig,data,NULL,NULL);
							}
						}

					}

					trig = trig->next;
				}

			}
		}

	} 
	else if (!strcmp(data->command,"JOIN"))
	{
		trig = net->trigs->join;

		if (data->rest != NULL)
		{
			snprintf(newmask,sizeof(newmask),"%s %s!%s@%s",data->rest[0],
					data->prefix->nick,
					data->prefix->user,
					data->prefix->host);

			if (!troll_matchwilds(newmask,trig->mask))
			{
				if (trig->handler != NULL)
				{
					trig->usecount++;
					trig->handler(net,trig,data,NULL,NULL);
				}
			}

		}

	}
	else if (!strcmp(data->command,"PART"))
	{
		trig = net->trigs->part;

		while (trig != NULL)
		{
			if ((data->rest != NULL) && data->prefix->nick != NULL && data->prefix->user != NULL && data->prefix->host != NULL)
			{
				snprintf(newmask,sizeof(newmask),"%s %s!%s@%s",data->rest[0],
						data->prefix->nick,
						data->prefix->user,
						data->prefix->host);

				if (!troll_matchwilds(newmask,trig->mask))
				{
					if (trig->handler != NULL)
					{
						trig->usecount++;
						trig->handler(net,trig,data,NULL,NULL);
						/* Stackable, do not break */
					}
				}

			}

			trig = trig->next;
		}
	}
	else if (!strcmp(data->command,"QUIT"))
	{

	}	
	else if (!strcmp(data->command,"TOPIC")) /* Eggdrop Compatible */
	{
		trig = net->trigs->topc;

		while (trig != NULL)
		{
			if ((data->c_params != NULL) && data->c_params[0] != NULL && data->rest_str != NULL)
			{
				snprintf(newmask,sizeof(newmask),"%s %s",data->c_params[0],data->rest_str);

				if (!troll_matchwilds(newmask,trig->mask))
				{
					if (trig->handler != NULL)
					{
						trig->usecount++;
						trig->handler(net,trig,data,NULL,NULL);
						/* Stackable, do not break */
					}
				}

			}

			trig = trig->next;
		}
	}
	else if (!strcmp(data->command,"KICK")) /* Eggdrop Compatible */
	{
		trig = net->trigs->kick;

		while (trig != NULL)
		{
			if ((data->c_params != NULL) && data->c_params[0] != NULL && data->c_params[1] != NULL)
			{
				snprintf(newmask,sizeof(newmask),"%s %s %s",data->c_params[0],data->c_params[1],data->rest_str);

				if (!troll_matchwilds(newmask,trig->mask))
				{
					if (trig->handler != NULL)
					{
						trig->usecount++;
						trig->handler(net,trig,data,NULL,NULL);
						/* Stackable, do not break */
					}
				}

			}

			trig = trig->next;
		}
	}
	else if (!strcmp(data->command,"NOTICE")) /* Needs verified as Eggdrop Compatible */
	{
		/* Check bind NOTC, STACKABLE */
		trig = net->trigs->notc;

		while (trig != NULL)
		{
			if (data->rest[0] != NULL)
			{
				if (!troll_matchwilds(data->rest_str,trig->mask))
				{
					if (trig->handler != NULL)
					{
						trig->usecount++;
						trig->handler(net,trig,data,NULL,NULL);
						/* Stackable, do not break */
					}
				}
			}

			trig = trig->next;
		}

	}
	else
	{
		/* Check for RAW */
		trig = net->trigs->raw;

		while (trig != NULL)
		{
			if (!strcmp(trig->mask,data->command))
			{
				trig->usecount++;
				trig->handler(net,trig,data,NULL,NULL);
			}

			trig = trig->next;
		}
	}

	return;

}

struct trig_table *new_trig_table(void)
{
	struct trig_table *ret;

	ret = tmalloc(sizeof(struct trig_table));

	ret->pub       = NULL;
	ret->pubm      = NULL;
	ret->msg       = NULL;
	ret->msgm      = NULL;
	ret->join      = NULL;
	ret->part      = NULL;
	ret->sign      = NULL;
	ret->ctcp      = NULL;
	ret->kick      = NULL;
	ret->notc      = NULL;
	ret->dcc       = NULL;
	ret->raw       = NULL;
	ret->topc      = NULL;

	return ret;
}


/*

	 struct trigger
	 {
	 char *glob_flags;
	 char *chan_flags;

	 int type;

	 char *mask;  

	 void (*handler)(struct network *, struct trigger *, struct irc_data *, struct dcc_session *, const char *);

	 char *command;

	 int usecount;

	 struct trigger *prev;
	 struct trigger *next;  
	 };*/

void free_trigger(struct trigger *trig)
{
	free(trig->glob_flags);
	free(trig->chan_flags);
	free(trig->mask);
	free(trig->command);
	free(trig);
}


void free_trigger_table(struct trig_table *table){
	if (table->pub){  free_trigger_list(table->pub);  }
	if (table->pubm){  free_trigger_list(table->pubm);  }
	if (table->msg){  free_trigger_list(table->msg);  }
	if (table->msgm){  free_trigger_list(table->msgm);  }
	if (table->join){  free_trigger_list(table->join);  }
	if (table->part){  free_trigger_list(table->part);  }
	if (table->sign){  free_trigger_list(table->sign);  }
	if (table->ctcp){  free_trigger_list(table->ctcp);  }
	if (table->kick){  free_trigger_list(table->kick);  }
	if (table->notc){  free_trigger_list(table->notc);  }
	if (table->dcc){  free_trigger_list(table->dcc);  }
	if (table->raw){  free_trigger_list(table->raw);  }
	if (table->topc){	free_trigger_list(table->topc); }
	free(table);
}

void free_trigger_list(struct trigger *list)
{
	struct trigger *next=NULL;

	while (list != NULL)
	{
		next = list->next;
		free_trigger(list);
		list = next;
	}
}

