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
#include <string.h>
#include <unistd.h>
#include <sys/select.h>
#include <netdb.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <errno.h>

#include "main.h"
#include "irc.h"
#include "dcc.h"
#include "user.h"
#include "network.h"
#include "egg_lib.h"
#include "trigger.h"
#include "sockets.h"
#include "log_entry.h"

struct dcc_session *dcc_list_del(struct dcc_session *dccs, struct dcc_session *del)
{
	struct dcc_session *tmp = NULL;

	if ((tmp = dccs) == NULL)
	{
		log_entry_printf(NULL,NULL,"T","dcc_list_del() called with NULL dcc_session list");
		return NULL;
	}

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

	log_entry_printf(NULL,NULL,"T","dcc_list_del() called with a dcc_session deletion that no entry existed for");

	return dccs;
}

struct dcc_session *dcc_list_add(struct dcc_session *dccs, struct dcc_session *add)
{
	struct dcc_session *tmp = NULL;

	if ((tmp = dccs) == NULL)
		return add;

	while (tmp->next != NULL) tmp = tmp->next;

	tmp->next = add;
	add->prev = tmp;

	return dccs;
}


void dcc_init_listener(struct network *net)
{
	char *dcchostip;
	char *dcchost;
	struct sockaddr_in dccaddr;
	struct hostent *he;
	int yes=1;

	if (net->vhost == NULL)
	{
		if (net->shost == NULL)
		{
			troll_debug(LOG_WARN,"Neither a valid vhost, nor a valid server host exists for a DCC connection");
			return;
		} 
	}

	dcchost   = (net->vhost != NULL) ? net->vhost : net->shost;
	dcchostip = NULL;

	if ((he = gethostbyname(dcchost)) == NULL)
	{
		troll_debug(LOG_WARN,"Could not resolve host (%s) for DCC listening",dcchost);
		return;
	}

	dcchostip = tmalloc0(3*4+3+1);
	sprintf(dcchostip,"%s",inet_ntoa(*((struct in_addr *)he->h_addr)));

	if ((net->dcc_listener = socket(PF_INET, SOCK_STREAM, 0)) == -1) 
	{
		troll_debug(LOG_ERROR,"Could not create socket for DCC listener");
		return;
	}

	if (setsockopt(net->dcc_listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) 
	{
		troll_debug(LOG_ERROR,"Could not set socket options");
		return;
	}

	dccaddr.sin_family = AF_INET;

	dccaddr.sin_addr.s_addr = inet_addr(dcchostip);

	if (net->dcc_port == -1)
	{
		net->dcc_port = 4928;
	}

	dccaddr.sin_port = htons(net->dcc_port);

	memset(&(dccaddr.sin_zero), '\0', 8);

	if (bind(net->dcc_listener, (struct sockaddr *)&dccaddr, sizeof(dccaddr)) == -1) 
	{
		troll_debug(LOG_ERROR,"Could not bind to DCC socket");
		free(dcchostip);
		return;
	}

	if (listen(net->dcc_listener, DCC_MAX) == -1) 
	{
		troll_debug(LOG_ERROR,"Could not listen on DCC socket");
		free(dcchostip);
		return;
	}

	troll_debug(LOG_DEBUG,"Listening on %s port %d\n",dcchostip,net->dcc_port);
	free(dcchostip);

	return;
}

/* This creates a new connection from a listening socket */
void new_dcc_connection(struct network *net)
{
	struct dcc_session *newdcc;
	struct dcc_session *tmpdcc;
	struct sockaddr_in client_addr;
	socklen_t sin_size = 0;
	int sock = 0;

	if ((sock = accept(net->dcc_listener,(struct sockaddr *)&client_addr,&sin_size)) == -1)
	{
		return;
	}

	sin_size = sizeof(struct sockaddr_in);

	newdcc = new_dcc_session();

	newdcc->sock   = sock;
	newdcc->status = DCC_NOTREADY;

	if (net->dccs == NULL)
		net->dccs = newdcc;
	else
	{
		tmpdcc = net->dccs;

		while (tmpdcc->next != NULL)
			tmpdcc = tmpdcc->next;

		tmpdcc->next = newdcc;
		newdcc->prev = tmpdcc;
	}

	newdcc->net = net;

	irc_printf(newdcc->sock,"Welcome to Trollbot");
	irc_printf(newdcc->sock,"enter your username to continue.");

	return; 
}

struct dcc_session *new_dcc_session(void)
{
	struct dcc_session *ret;

	ret = tmalloc(sizeof(struct dcc_session));

	/* Needs proper ID */
	ret->id   = -1;
	ret->sock = -1;
	ret->user = NULL;
	ret->status = DCC_WAITING;

	ret->prev = NULL;
	ret->next = NULL;
	return ret;
}

void free_dcc_sessions(struct dcc_session *dccs)
{
	struct dcc_session *tmp = NULL;

	while (dccs != NULL)
	{
		tmp  = dccs->next;
		free_dcc_session(dccs);
		dccs = tmp;
	}
}

void free_dcc_session(struct dcc_session *dcc)
{
	/* et voila */
	free(dcc);
}

void reverse_dcc_chat(struct network *net, struct trigger *trig, struct irc_data *data, struct dcc_session *dcc, const char *dccbuf)
{
	struct hostent *he;
	char *dcchostip;

	if (net->shost == NULL)
	{
		if (net->vhost == NULL)
		{
			irc_printf(net->sock,"PRIVMSG %s :No suitable IP/Port combination found.",data->prefix->nick);
			return;
		} else
			net->shost = tstrdup(net->vhost);
	}

	if ((he = gethostbyname(net->shost)) == NULL)
	{
		troll_debug(LOG_WARN,"Could not resolve host (%s) for DCC message",net->shost);
		return;
	}

	dcchostip = tmalloc0(3*4+3+1);
	sprintf(dcchostip,"%s",inet_ntoa(*((struct in_addr *)he->h_addr)));

	irc_printf(net->sock,"PRIVMSG %s :\001DCC CHAT chat %d %d\001",data->prefix->nick,htonl(inet_addr(dcchostip)),net->dcc_port);
}

void initiate_dcc_chat(struct network *net, struct trigger *trig, struct irc_data *data, struct dcc_session *dcc, const char *dccbuf)
{
	char *ipStr=NULL;
	int dcc_sock = -1;
	unsigned long ip = 0;
	unsigned int port = 0;
	int argc;
	int highest_idx = 0;
	struct sockaddr_in req_addr;
	struct dcc_session *newdcc;
	struct dcc_session *tmp;

	/* Make sure we have at least the required args */
	if (data->rest == NULL)
		return;

	for(argc=0;argc<5;argc++)
	{
		if (data->rest[argc] == NULL)
			return;
	}

	ip = strtoul(data->rest[3],(char **)'\0',10);
	port = atoi(data->rest[4]);

	if ((dcc_sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		troll_debug(LOG_WARN,"Could not create DCC socket");
		return;
	}

	socket_set_nonblocking(dcc_sock);

	req_addr.sin_family      = AF_INET;
	req_addr.sin_addr.s_addr = htonl(ip);
	req_addr.sin_port        = htons(port);

	ipStr = inet_ntoa(req_addr.sin_addr);

	memset(&(req_addr.sin_zero), '\0', 8);

	if (connect(dcc_sock,(struct sockaddr *)&req_addr,sizeof(struct sockaddr)) == -1)
	{
		if (errno == EINPROGRESS)
		{
			troll_debug(LOG_DEBUG,"Non-blocking connect(%s) in progress", ipStr);
		}
		else
		{
			troll_debug(LOG_WARN,"Could not connect to dcc user at %d",port);
			return;
		}
	}

	/* We're not sure if we're connected at this point, let's make a struct for
	 * it anyways
	 */    
	newdcc = new_dcc_session();

	newdcc->sock   = dcc_sock;

	/* Mark it so the select() loop knows what to do */
	newdcc->status = DCC_NONBLOCKCONNECT;

	/* So we know which userdb to read from */
	newdcc->net    = net; 

	/* Insert it into the global DCC list */
	if (net->dccs == NULL)
	{
		newdcc->id     = 1;
		net->dccs = newdcc;
		return;
	}

	tmp = net->dccs;

	while (tmp->prev != NULL)
		tmp = tmp->prev;

	highest_idx = tmp->id;

	while (tmp->next != NULL)
	{
		highest_idx = (tmp->id > highest_idx) ? tmp->id : highest_idx;
		tmp = tmp->next;
	}

	/* give the largest id + 1 to the new dcc session */
	newdcc->id = ++highest_idx;

	tmp->next = newdcc;

	newdcc->prev = tmp;

	return;
}

void dcc_help_menu(struct network *net, struct trigger *trig, struct irc_data *data, struct dcc_session *dcc, const char *dccbuf)
{
	irc_printf(dcc->sock,"TrollBot v1.0 DCC MENU\n"
			"----------------------\n\n"
			" +chan -chan +user -user\n"
			" chattr rehash tcl tbinds\n"
			" msg\n");

	return;

}

int dcc_in(struct dcc_session *dcc)
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
		recved = recv(dcc->sock,buffer,BUFFER_SIZE-1,0);
	} else {
		/* There was a fragment left over */
		buffer = tcrealloc0(buffer,
				strlen(buffer) + BUFFER_SIZE + 1,
				&size);

		recved = recv(dcc->sock,&buffer[strlen(buffer)],BUFFER_SIZE-1,0);

	}

	switch (recved)
	{
		case -1:
			free(buffer);
			buffer = NULL;
			return 1;
		case 0:
			dcc->sock = -1;
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

		/* This should deal with ircds which output \r only, \r\n, or \n */
		while (*ptr == '\r' || *ptr == '\n')
			ptr++;

		parse_dcc_line(dcc,line);

		free(line);

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

void dcc_command_handler(struct dcc_session *dcc, const char *command)
{
	struct trigger *trig;

	trig = dcc->net->trigs->dcc;

	while (trig != NULL)
	{
		/* We want to check user flags for this since they authenticated */
		if ((trig->glob_flags == NULL) || egg_matchattr(dcc->net,dcc->user->username,trig->glob_flags,NULL))
		{
			if (!strncmp(trig->mask,command,strlen(trig->mask)))
			{
				if (trig->handler != NULL)
				{
					trig->usecount++;
					trig->handler(dcc->net,trig,NULL,dcc,command);
					return;
				}
			}
		}

		trig = trig->next;
	}

	dcc_partyline_handler(dcc,command);

	return;
}

void dcc_partyline_handler(struct dcc_session *dcc, const char *message)
{
	struct dcc_session *tmp;

	if ((tmp = dcc->net->dccs) == NULL)
		return;

	while (tmp != NULL)
	{
		irc_printf(tmp->sock,"<%s> %s",dcc->user->username,message);

		tmp = tmp->next;
	}

	return;
}

void dcc_print_motd(struct dcc_session *dcc)
{
	FILE *motd;
	char *fbuf;
	size_t count = 0;
	size_t i     = 0;

	if (dcc->net->dcc_motd == NULL)
	{
		if (g_cfg->dcc_motd == NULL)
		{
			troll_debug(LOG_WARN, "No global specified for DCC MOTD, messed up defaults.conf?");
			irc_printf(dcc->sock, "Error: No MOTD found");
			return;
		}

		if ((motd = fopen(g_cfg->dcc_motd,"r")) == NULL)
		{
			troll_debug(LOG_WARN, "Could not open dcc MOTD file: %s\n",g_cfg->dcc_motd);
			irc_printf(dcc->sock, "Error: Could not read MOTD file");
			return;
		}	
	}
	else
	{
		/* Network Specific DCC MOTD exists */
		if ((motd = fopen(dcc->net->dcc_motd,"r")) == NULL)
		{
			troll_debug(LOG_WARN, "Could not open dcc MOTD file: %s\n",dcc->net->dcc_motd);
			irc_printf(dcc->sock, "Error: Could not read MOTD file");
			return;
		}
	}

	fbuf = tmalloc0(1024);

	/* Read the entire file into memory */
	for(i=0;(count = fread(&fbuf[i],1,1024,motd)) == 1024;i+=1024)
	{
		if ((fbuf = realloc(fbuf,i+1024+1024)) == NULL)
		{
			troll_debug(LOG_FATAL,"Could not allocate memory for DCC MOTD file, barfing.\n");
			exit(EXIT_FAILURE);
		}
	}

	/* Terminate it with a NULL */
	fbuf[i+count] = '\0';

	/* FIXME: Make sure this sends the entire payload */
	irc_printf(dcc->sock,"%s",fbuf);

	fclose(motd);

	return;
}

void parse_dcc_line(struct dcc_session *dcc, const char *buffer)
{
	struct user *user;

	switch (dcc->status)
	{
		case DCC_CONNECTED:
			user = dcc->net->users;

			while (user != NULL)
			{
				if (!strcmp(user->username,buffer))
				{
					if (!egg_matchattr(dcc->net,user->username,"p",NULL))
					{
						irc_printf(dcc->sock,"You do not have the flags to access DCC.");
						close(dcc->sock);
						dcc->sock = -1;
						dcc->net->dccs = dcc_list_del(dcc->net->dccs,dcc);
						free_dcc_session(dcc);
						return;
					}
					dcc->user = user;
					irc_printf(dcc->sock,"Please enter your password.");
					dcc->status = DCC_HAS_USERNAME;
					break;
				}

				user = user->next;
			}

			if (user == NULL)
			{
				irc_printf(dcc->sock,"Incorrect username.");
				close(dcc->sock);
				dcc->sock = -1;
				dcc->net->dccs = dcc_list_del(dcc->net->dccs, dcc);
				free_dcc_session(dcc);
			}

			break;
		case DCC_HAS_USERNAME:
			/* No password */
			if (dcc->user->passhash == NULL)
			{
				irc_printf(dcc->sock,"Login failed");
				close(dcc->sock);
				dcc->sock = -1;
				dcc->net->dccs = dcc_list_del(dcc->net->dccs, dcc);
				free_dcc_session(dcc);
			}

			if (egg_passwdok(dcc->net,dcc->user->username,buffer))
			{
				/* Successfully Authenticated */

				/* Convert Hash if global requested hash type has changed 
				 * while we still have the plaintext pass.
				 */
				if (strcmp(g_cfg->hash_type,dcc->user->hash_type))
				{
					free(dcc->user->hash_type);
					dcc->user->hash_type = tstrdup(g_cfg->hash_type);

					free(dcc->user->passhash);
					dcc->user->passhash = egg_makepasswd(buffer, g_cfg->hash_type);

					users_save(dcc->net);
				}

				/* Print net specific dcc_motd if exists, if not, use global */
				dcc_print_motd(dcc);

				irc_printf(dcc->sock,"Type .help for help.");
				dcc->status = DCC_AUTHENTICATED;
			}
			else
			{
				irc_printf(dcc->sock,"Incorrect password");
				close(dcc->sock);
				dcc->sock = -1;
				dcc->net->dccs = dcc_list_del(dcc->net->dccs, dcc);
				free_dcc_session(dcc);
			}

			break;
		case DCC_AUTHENTICATED:
			dcc_command_handler(dcc, buffer);
			break;
	}

	return;
}

void dcc_add_chan(struct network *net, struct trigger *trig, struct irc_data *data, struct dcc_session *dcc, const char *dccbuf)
{
	/* hack */
	irc_printf(dcc->net->sock,"JOIN %s",egg_makearg(dccbuf,trig->mask));
}

void dcc_del_chan(struct network *net, struct trigger *trig, struct irc_data *data, struct dcc_session *dcc, const char *dccbuf)
{
	irc_printf(dcc->net->sock,"PART %s",egg_makearg(dccbuf,trig->mask));
}

void dcc_rehash(struct network *net, struct trigger *trig, struct irc_data *data, struct dcc_session *dcc, const char *dccbuf)
{
	/* Crude rehash mechanism */
}

void dcc_who(struct network *net, struct trigger *trig, struct irc_data *data, struct dcc_session *dcc, const char *dccbuf)
{
	struct dcc_session *dtmp;

	dtmp = dcc->net->dccs;

	irc_printf(dcc->sock,"Users currently connected:");

	while (dtmp != NULL)
	{
		if (dtmp->user != NULL)
			irc_printf(dcc->sock,"Username: %s Host: %s",dtmp->user->username,dtmp->user->uhost);

		dtmp = dtmp->next;
	}

	return;
}

void dcc_chattr(struct network *net, struct trigger *trig, struct irc_data *data, struct dcc_session *dcc, const char *dccbuf)
{
	/*	char **args;

			args = tssv_split(egg_makearg(dccbuf,trig->mask));

	 * BS * 
	 if (args != NULL)
	 if (args[0] != NULL)
	 if (args[1] != NULL)
	 if (args[2] != NULL)	
	 egg_chattr(net,args[0],args[1],args[2]);
	 */
}

void dcc_tbinds(struct network *net, struct trigger *trig, struct irc_data *data, struct dcc_session *dcc, const char *dccbuf)
{
	struct trigger *tmp_trig;


	irc_printf(dcc->sock,"Trollbot bind table:");

	irc_printf(dcc->sock,"bind PUB:");
	tmp_trig = net->trigs->pub;

	if (tmp_trig == NULL) irc_printf(dcc->sock,"None.");

	while (tmp_trig != NULL)
	{
		if (tmp_trig->command != NULL)
			irc_printf(dcc->sock,"Mask: (%s) Usecount: %d Command: (%s)",tmp_trig->mask,tmp_trig->usecount,tmp_trig->command);
		else
			irc_printf(dcc->sock,"Mask: (%s) Usecount: %d Command: Internal",tmp_trig->mask,tmp_trig->usecount);

		tmp_trig = tmp_trig->next;
	} 


	irc_printf(dcc->sock,"bind PUBM:");
	tmp_trig = net->trigs->pubm;

	if (tmp_trig == NULL) irc_printf(dcc->sock,"None.");

	while (tmp_trig != NULL)
	{    
		if (tmp_trig->command != NULL)
			irc_printf(dcc->sock,"Mask: (%s) Usecount: %d Command: (%s)",tmp_trig->mask,tmp_trig->usecount,tmp_trig->command);
		else
			irc_printf(dcc->sock,"Mask: (%s) Usecount: %d Command: Internal",tmp_trig->mask,tmp_trig->usecount);     

		tmp_trig = tmp_trig->next;
	}   


	irc_printf(dcc->sock,"bind MSG:");
	tmp_trig = net->trigs->msg;

	if (tmp_trig == NULL) irc_printf(dcc->sock,"None.");

	while (tmp_trig != NULL)
	{    
		if (tmp_trig->command != NULL)
			irc_printf(dcc->sock,"Mask: (%s) Usecount: %d Command: (%s)",tmp_trig->mask,tmp_trig->usecount,tmp_trig->command);
		else
			irc_printf(dcc->sock,"Mask: (%s) Usecount: %d Command: Internal",tmp_trig->mask,tmp_trig->usecount);     

		tmp_trig = tmp_trig->next;
	}   


	irc_printf(dcc->sock,"bind MSGM:");
	tmp_trig = net->trigs->msgm;

	if (tmp_trig == NULL) irc_printf(dcc->sock,"None.");

	while (tmp_trig != NULL)
	{    
		if (tmp_trig->command != NULL)
			irc_printf(dcc->sock,"Mask: (%s) Usecount: %d Command: (%s)",tmp_trig->mask,tmp_trig->usecount,tmp_trig->command);
		else
			irc_printf(dcc->sock,"Mask: (%s) Usecount: %d Command: Internal",tmp_trig->mask,tmp_trig->usecount);     

		tmp_trig = tmp_trig->next;
	}   


	irc_printf(dcc->sock,"bind JOIN:");
	tmp_trig = net->trigs->join;

	if (tmp_trig == NULL) irc_printf(dcc->sock,"None.");

	while (tmp_trig != NULL)
	{    
		if (tmp_trig->command != NULL)
			irc_printf(dcc->sock,"Mask: (%s) Usecount: %d Command: (%s)",tmp_trig->mask,tmp_trig->usecount,tmp_trig->command);
		else
			irc_printf(dcc->sock,"Mask: (%s) Usecount: %d Command: Internal",tmp_trig->mask,tmp_trig->usecount);     

		tmp_trig = tmp_trig->next;
	}   


	irc_printf(dcc->sock,"bind PART:");
	tmp_trig = net->trigs->part;

	if (tmp_trig == NULL) irc_printf(dcc->sock,"None.");

	while (tmp_trig != NULL)
	{    
		if (tmp_trig->command != NULL)
			irc_printf(dcc->sock,"Mask: (%s) Usecount: %d Command: (%s)",tmp_trig->mask,tmp_trig->usecount,tmp_trig->command);
		else
			irc_printf(dcc->sock,"Mask: (%s) Usecount: %d Command: Internal",tmp_trig->mask,tmp_trig->usecount);     

		tmp_trig = tmp_trig->next;
	}

	irc_printf(dcc->sock,"bind DCC:");
	tmp_trig = net->trigs->dcc;

	if (tmp_trig == NULL) irc_printf(dcc->sock,"None.");

	while (tmp_trig != NULL)
	{
		if (tmp_trig->command != NULL)
			irc_printf(dcc->sock,"Mask: (%s) Usecount: %d Command: (%s)",tmp_trig->mask,tmp_trig->usecount,tmp_trig->command);
		else
			irc_printf(dcc->sock,"Mask: (%s) Usecount: %d Command: Internal",tmp_trig->mask,tmp_trig->usecount);

		tmp_trig = tmp_trig->next;
	}


	return;
}
