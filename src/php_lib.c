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


PHP_FUNCTION(putdcc)
{
  long idx;
  char *message;
  int message_len;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ls",  &idx,
                                                              &message,
                                                              &message_len) == FAILURE)
  {
    RETURN_FALSE;
  }

  egg_putdcc((int)idx,message);

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

