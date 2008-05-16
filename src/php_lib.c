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

/*
 * Zend/zend.h:#define INTERNAL_FUNCTION_PARAMETERS int ht, zval *return_value, zval **return_value_ptr, zval *this_ptr, int return_value_used TSRMLS_DC
 * WTF this actually gets parameters as
 */


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

	RETURN_UTF8_STRING((char *)user->nick, ZSTR_DUPLICATE);
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

