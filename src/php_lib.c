#include "main.h"
#include "php_lib.h"

#include <main/php.h>
#include <main/SAPI.h>
#include <main/php_main.h>
#include <main/php_variables.h>
#include <main/php_ini.h>
#include <zend_ini.h>
#undef END_EXTERN_C

#include "config_engine.h"
#include "php_embed.h"
#include "trigger.h"
#include "network.h"
#include "egg_lib.h"
#include "irc.h"
#include "user.h"
#include "t_timer.h"

/* ispermban <ban> [channel] */
/* Semantics: this will return TRUE or FALSE instead of 1 or 0 */
PHP_FUNCTION(ispermban)
{
	struct network *net;
	char *network;
	int network_len;
	char *ban;
	int ban_len;
	char *channel;
	int channel_len;
	int ret = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss|s", &network, &network_len, &ban, &ban_len, &channel, &channel_len))
	{
		RETURN_FALSE;
	}

	net = g_cfg->networks;	

	while (net != NULL)
	{
		if (!tstrcasecmp(net->label,network))
			break;
		
		net = net->next;
	}	

	if (net == NULL)
		RETURN_FALSE;

	ret = egg_ispermban(net, ban, channel);

	if (ret == 1)
	{
		RETURN_TRUE;
	}
	else
	{
		RETURN_FALSE;
	}
}

/* matchattr <handle> <flags> [channel] */
PHP_FUNCTION(matchattr)
{
	struct network *net;
	char *network;
	int network_len;
	char *handle;
	int handle_len;
	char *flags;
	int flags_len;
	char *channel;
	int channel_len;
	int ret = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sss|s", &network, &network_len, &handle, &handle_len, &flags, &flags_len, &channel, &channel_len))
	{
		RETURN_FALSE;
	}

	net = g_cfg->networks;	

	while (net != NULL)
	{
		if (!tstrcasecmp(net->label,network))
			break;
		
		net = net->next;
	}	

	if (net == NULL)
		RETURN_FALSE;

	ret = egg_matchattr(net, handle, flags, channel);

	RETURN_LONG(ret);
}

/*
 *   isbansticky <ban> [channel]
 *   Returns: 1 if the specified ban is marked as sticky in the global ban
 *   list; 0 otherwise. If a channel is specified, that channel's ban list
 *   is checked as well.
 *   Module: channels
 */
/* Semantics: This will return TRUE or FALSE rather than eggdrop's 1 or 0 */
PHP_FUNCTION(isbansticky)
{
	struct network *net;
	char *network;
	int network_len;
	char *ban;
	int ban_len;
	char *channel;
	int channel_len;
	int ret = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss|s", &network, &network_len, &ban, &ban_len, &channel, &channel_len))
	{
		RETURN_FALSE;
	}

	net = g_cfg->networks;	

	while (net != NULL)
	{
		if (!tstrcasecmp(net->label,network))
			break;
		
		net = net->next;
	}	

	if (net == NULL)
		RETURN_FALSE;

	ret = egg_isbansticky(net, ban, channel);

	if (ret == 1)
	{
		RETURN_TRUE;
	}
	else
	{
		RETURN_FALSE;
	}
}



/* isban <ban> [channel] */
/* Semantics: this will return TRUE or FALSE instead of 1 or 0 */
PHP_FUNCTION(isban)
{
	struct network *net;
	char *network;
	int network_len;
	char *ban;
	int ban_len;
	char *channel;
	int channel_len;
	int ret = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss|s", &network, &network_len, &ban, &ban_len, &channel, &channel_len))
	{
		RETURN_FALSE;
	}

	net = g_cfg->networks;	

	while (net != NULL)
	{
		if (!tstrcasecmp(net->label,network))
			break;
		
		net = net->next;
	}	

	if (net == NULL)
		RETURN_FALSE;

	ret = egg_isban(net, ban, channel);

	if (ret == 1)
	{
		RETURN_TRUE;
	}
	else
	{
		RETURN_FALSE;
	}
}

PHP_FUNCTION(unbind)
{
	char *network;
	int network_len;
	char type;
	int type_len;
	char *flags;
	int flags_len;
	char *mask;
	int mask_len;
	char *command;
	int command_len;
	struct network *net;


	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sssss", &network, &network_len, &type, &type_len, &flags, &flags_len, &mask, &mask_len, &command, &command_len))
	{
		RETURN_FALSE;
	}

	net = g_cfg->networks;	

	while (net != NULL)
	{
		if (!tstrcasecmp(net->label,network))
			break;
		
		net = net->next;
	}	

	if (net == NULL)
		RETURN_FALSE;

	egg_unbind(net, type, flags, mask, command);

	RETURN_TRUE;
}

/*
 * Zend/zend.h:#define INTERNAL_FUNCTION_PARAMETERS int ht, zval *return_value, zval **return_value_ptr, zval *this_ptr, int return_value_used TSRMLS_DC
 * WTF this actually gets parameters as
 */
PHP_FUNCTION(utimer)
{
	char *network;
	int network_len;
	char *evalstr;
	int evalstr_len;
	long seconds = 1;
	struct network *net;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sls", &network, &network_len, &seconds, &evalstr, &evalstr_len))
	{
		RETURN_FALSE;
	}

	net = g_cfg->networks;	

	while (net != NULL)
	{
		if (!tstrcasecmp(net->label,network))
			break;
		
		net = net->next;
	}	

	if (net == NULL)
		RETURN_FALSE;

	egg_utimer(net, (int)seconds, evalstr, t_timer_php_handler);

	RETURN_TRUE;
}

PHP_FUNCTION(botnick)
{
	char *network;
	int network_len;
	struct network *net;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &network, &network_len))
	{
		RETURN_FALSE;
	}

	net = g_cfg->networks;	

	while (net != NULL)
	{
		if (!tstrcasecmp(net->label,network))
			break;
		
		net = net->next;
	}	

	if (net == NULL)
		RETURN_FALSE;

	RETURN_STRING((char *)net->botnick, 1);
}

PHP_FUNCTION(chhandle)
{
	char *network;
	int network_len;
	char *oldh;
	int oldh_len;
	char *newh;
	int newh_len;
	struct network *net;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sss", &network, &network_len, &oldh, &oldh_len, &newh, &newh_len))
	{
		RETURN_FALSE;
	}

	net = g_cfg->networks;	

	while (net != NULL)
	{
		if (!tstrcasecmp(net->label,network))
			break;
		
		net = net->next;
	}	

	if (net == NULL)
		RETURN_FALSE;

	RETURN_LONG(egg_chhandle(net, oldh, newh));
}

PHP_FUNCTION(passwdok)
{
	char *network;
	int network_len;
	char *passwd;
	int passwd_len;
	char *user;
	int user_len;
	struct network *net;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sss", &network, &network_len, &user, &user_len, &passwd, &passwd_len))
	{
		RETURN_FALSE;
	}
	
	net = g_cfg->networks;

	while (net != NULL)
	{
		if (!tstrcasecmp(net->label,network))
			break;
		
		net = net->next;
	}	

	if (net == NULL)
		RETURN_FALSE;

	RETURN_LONG(egg_passwdok(net, user, passwd));
}

PHP_FUNCTION(save)
{
	char *network;
	int network_len;
	struct network *net;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &network, &network_len))
	{
		RETURN_FALSE;
	}

	net = g_cfg->networks;

	while (net != NULL)
	{
		if (!tstrcasecmp(net->label,network))
			break;
		
		net = net->next;
	}	

	if (net == NULL)
		RETURN_FALSE;

	egg_save(net);

	RETURN_TRUE;
}


PHP_FUNCTION(countusers)
{
	char *network;
	int network_len;
	struct network *net;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &network, &network_len))
	{
		RETURN_FALSE;
	}

	net = g_cfg->networks;

	while (net != NULL)
	{
		if (!tstrcasecmp(net->label,network))
			break;
		
		net = net->next;
	}	

	if (net == NULL)
		RETURN_FALSE;

	RETURN_LONG(egg_countusers(net));
}

PHP_FUNCTION(savechannels)
{
	char *network;
	int  network_len;
	struct network *net;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &network, &network_len))
	{
		RETURN_FALSE;
	}
	
	net = g_cfg->networks;

	while (net != NULL)
	{
		if (!strcmp(net->label,network))
			break;

		net = net->next;
	}

	/* Need to differentiate between error and 0 */
	if (net == NULL)
		RETURN_FALSE;


	egg_savechannels(net);
	
	RETURN_TRUE;
}
	
PHP_FUNCTION(finduser)
{
	char *network;
	int  network_len;
	char *whom;
	int   whom_len;
	struct network *net;
	struct user *user;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss", &network, &network_len, &whom, &whom_len))
	{
		/* Need to differentiate between error and 0 */
		RETURN_FALSE;
	}

	net = g_cfg->networks;

	while (net != NULL)
	{
		if (!tstrcasecmp(net->label,network))
			break;

		net = net->next;
	}

	/* Need to differentiate between error and 0 */
	if (net == NULL)
		RETURN_FALSE;

	if ((user = egg_finduser(net, whom)) == NULL)
		RETURN_FALSE;

	RETURN_STRING((char *)user->nick, 1);
}

PHP_FUNCTION(validuser)
{
	char *network;
	int  network_len;
	char *whom;
	int   whom_len;
	struct network *net;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss", &network, &network_len, &whom, &whom_len))
	{
		/* Need to differentiate between error and 0 */
		RETURN_FALSE;
	}

	net = g_cfg->networks;

	while (net != NULL)
	{
		if (!strcmp(net->label,network))
			break;

		net = net->next;
	}

	/* Need to differentiate between error and 0 */
	if (net == NULL)
		RETURN_FALSE;

	if (egg_validuser(net,whom))
	{
		RETURN_TRUE;
	}
	else
	{
		RETURN_FALSE;
	}
}

PHP_FUNCTION(putdcc)
{
	long idx;
	char *network;
	char *message;
	int network_len;
	int message_len;
	struct network *net;


	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sls", &network,
				&network_len,
				&idx,
				&message,
				&message_len) == FAILURE)
	{
		RETURN_FALSE;
	}

	net = g_cfg->networks;

	while (net != NULL)
	{
		if (!strcmp(net->label,network))
			break;

		net = net->next;
	}

	if (net == NULL)
		RETURN_FALSE;


	egg_putdcc(net,(int)idx,message);

	RETURN_TRUE;
}


/* This operates according to Eggdrop spec */
PHP_FUNCTION(matchwild)
{
	char *haystack;
	char *needle;
	int haystack_len;
	int needle_len;
	int ret;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss", &haystack,
				&haystack_len,
				&needle,
				&needle_len) == FAILURE)
	{
		RETURN_TRUE;
	}

	ret = egg_matchwilds(haystack,needle);

	if (ret)
	{
		RETURN_TRUE;
	}
	else
	{
		RETURN_FALSE;
	}

}


PHP_FUNCTION(bind)
{
	struct network *net;

	char *netw;
	char *type;
	char *flags;
	char *mask;
	char *func;

	int netw_len;
	int type_len;
	int flags_len;
	int mask_len;
	int func_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sssss", 
				&netw,   /* Network Name */
				&netw_len, 
				&type, 
				&type_len,
				&flags,
				&flags_len,
				&mask,
				&mask_len,
				&func,
				&func_len) == FAILURE)
	{
		RETURN_FALSE;
	}

	net = g_cfg->networks;

	while (net != NULL)
	{
		if (!strcmp(net->label,netw))
			break;

		net = net->next;
	}

	if (net == NULL)
		RETURN_FALSE;

	/* pass it off to egg_lib */
	if (!egg_bind(net,type,flags,mask,func,php_handler))
		RETURN_FALSE;

	RETURN_TRUE;
}

/* Need to figure out optional parameters in Zend
	 PHP_FUNCTION(matchattr)
	 {
	 struct network *net;

	 if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss", &netw, &netw_len, &msg, &msg_len) == FAILURE)
	 {
	 RETURN_FALSE;
	 }
	 */

PHP_FUNCTION(putserv)
{
	struct network *net;
	char *netw;
	int netw_len;
	char *msg;
	int msg_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss", &netw, &netw_len, &msg, &msg_len) == FAILURE)
	{
		RETURN_FALSE;
	}

	net = g_cfg->networks;

	while (net != NULL)
	{
		if (!strcmp(net->label,netw))
			break;

		net = net->next;
	}

	if (net == NULL)
		RETURN_FALSE;

	irc_printf(net->sock,msg);
}

