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

  if (!strcmp(type,"pub"))
  {
    if (net->trigs->pub == NULL)
    {
      net->trigs->pub       = new_trigger(flags,TRIG_PUB,mask,func,&php_handler);
      net->trigs->pub->prev = NULL;
      net->trigs->pub_head  = net->trigs->pub;
      net->trigs->pub_tail  = net->trigs->pub;
    }
    else
    {
      net->trigs->pub_tail->next = new_trigger(flags,TRIG_PUB,mask,func,&php_handler);
      net->trigs->pub            = net->trigs->pub_tail->next;
      net->trigs->pub->prev      = net->trigs->pub_tail;
      net->trigs->pub_tail       = net->trigs->pub;
    }

    net->trigs->pub_tail->next = NULL;
  }
  else if (!strcmp(type,"pubm"))
  {
    if (net->trigs->pubm == NULL)
    {
      net->trigs->pubm       = new_trigger(flags,TRIG_PUBM,mask,func,&php_handler);
      net->trigs->pubm->prev = NULL;
      net->trigs->pubm_head  = net->trigs->pubm;
      net->trigs->pubm_tail  = net->trigs->pubm;
    }
    else
    {
      net->trigs->pubm_tail->next = new_trigger(flags,TRIG_PUBM,mask,func,&php_handler);
      net->trigs->pubm            = net->trigs->pubm_tail->next;
      net->trigs->pubm->prev      = net->trigs->pubm_tail;
      net->trigs->pubm_tail       = net->trigs->pubm;
    }

    net->trigs->pubm_tail->next = NULL;
  }
  else if (!strcmp(type,"msg"))
  {
    if (net->trigs->msg == NULL)
    {
      net->trigs->msg       = new_trigger(flags,TRIG_MSG,mask,func,&php_handler);
      net->trigs->msg->prev = NULL;
      net->trigs->msg_head  = net->trigs->msg;
      net->trigs->msg_tail  = net->trigs->msg;
    }
    else
    {
      net->trigs->msg_tail->next = new_trigger(flags,TRIG_MSG,mask,func,&php_handler);
      net->trigs->msg            = net->trigs->msg_tail->next;
      net->trigs->msg->prev      = net->trigs->msg_tail;
      net->trigs->msg_tail       = net->trigs->msg;
    }

    net->trigs->msg_tail->next = NULL;

  }
  else if (!strcmp(type,"msgm"))
  {
    if (net->trigs->msgm == NULL)
    {
      net->trigs->msgm       = new_trigger(flags,TRIG_MSGM,mask,func,&php_handler);
      net->trigs->msgm->prev = NULL;
      net->trigs->msgm_head  = net->trigs->msgm;
      net->trigs->msgm_tail  = net->trigs->msgm;
    }
    else
    {
      net->trigs->msgm_tail->next = new_trigger(flags,TRIG_MSGM,mask,func,&php_handler);
      net->trigs->msgm            = net->trigs->msgm_tail->next;
      net->trigs->msgm->prev      = net->trigs->msgm_tail;
      net->trigs->msgm_tail       = net->trigs->msgm;
    }

    net->trigs->msgm_tail->next = NULL;

  }
  else if (!strcmp(type,"join"))
  {
    if (net->trigs->join == NULL)
    {
      net->trigs->join       = new_trigger(flags,TRIG_JOIN,mask,func,&php_handler);
      net->trigs->join->prev = NULL;
      net->trigs->join_head  = net->trigs->join;
      net->trigs->join_tail  = net->trigs->join;
    }
    else
    {
      net->trigs->join_tail->next = new_trigger(flags,TRIG_JOIN,mask,func,&php_handler);
      net->trigs->join            = net->trigs->join_tail->next;
      net->trigs->join->prev      = net->trigs->join_tail;
      net->trigs->join_tail       = net->trigs->join;
    }

    net->trigs->join_tail->next = NULL;

  }
  else if (!strcmp(type,"part"))
  {
    if (net->trigs->part == NULL)
    {
      net->trigs->part       = new_trigger(flags,TRIG_PART,mask,func,&php_handler);
      net->trigs->part->prev = NULL;
      net->trigs->part_head  = net->trigs->part;
      net->trigs->part_tail  = net->trigs->part;
    }
    else
    {
      net->trigs->part_tail->next = new_trigger(flags,TRIG_PART,mask,func,&php_handler);
      net->trigs->part            = net->trigs->part_tail->next;
      net->trigs->part->prev      = net->trigs->part_tail;
      net->trigs->part_tail       = net->trigs->part;
    }

    net->trigs->part_tail->next = NULL;
  }
  else if (!strcmp(type,"sign"))
  {
    if (net->trigs->sign == NULL)
    {
      net->trigs->sign       = new_trigger(flags,TRIG_SIGN,mask,func,&php_handler);
      net->trigs->sign->prev = NULL;
      net->trigs->sign_head  = net->trigs->sign;
      net->trigs->sign_tail  = net->trigs->sign;
    }
    else
    {
      net->trigs->sign_tail->next = new_trigger(flags,TRIG_SIGN,mask,func,&php_handler);
      net->trigs->sign            = net->trigs->sign_tail->next;
      net->trigs->sign->prev      = net->trigs->sign_tail;
      net->trigs->sign_tail       = net->trigs->sign;
    }

    net->trigs->sign_tail->next = NULL;
  }
  else
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

