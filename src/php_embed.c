/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2007 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Edin Kadribasic <edink@php.net>                              |
   +----------------------------------------------------------------------+
*/
/* $Id: php_embed.c,v 1.11.2.1.2.1 2007/01/01 09:36:12 sebastian Exp $ */

#include "main.h"
#include "php_embed.h"

#include <main/php.h>
#include <main/SAPI.h>
#include <main/php_main.h>
#include <main/php_variables.h>
#include <main/php_ini.h>
#include <zend_ini.h>

#include "php_lib.h"
#undef END_EXTERN_C

#ifdef PHP_WIN32
#include <io.h>
#include <fcntl.h>
#endif

#include "irc.h"
#include "trigger.h"
#include "network.h"
#include "egg_lib.h"

/* This function loads the PHP interpreter if it doesn't exist already.
 * then it executes the PHP file
 */
void myphp_eval_file(char *filename)
{
  zend_file_handle  file_handle;
  static int has_started = 0;

  TSRMLS_FETCH();

  if (has_started == 0)
  {
    php_embed_init(0,NULL PTSRMLS_DC);
    has_started = 1;
  }

  file_handle.type          = ZEND_HANDLE_FILENAME;
  file_handle.filename      = filename;
  file_handle.free_filename = 0;
  file_handle.opened_path   = NULL;

  zend_try 
  {

    if (zend_execute_scripts(ZEND_REQUIRE TSRMLS_CC, NULL, 1, &file_handle) != SUCCESS)
      troll_debug(LOG_WARN,"PHP Script (%s) could not be run",filename);
    else
      troll_debug(LOG_DEBUG,"PHP Script (%s) successfully loaded",filename);
  } zend_end_try();

  return;
}


void php_handler(struct network *net, struct trigger *trig, struct irc_data *data)
{
  zval *func;
  zval *netw;
  zval *nick;
  zval *uhost;
  zval *hand;
  zval *chan;
  zval *arg;
  zval *ret;
  zval *php_args[10]; /* may need changed */

  /* We have to create the arguments for the called function as zvals */
  TSRMLS_FETCH();

  switch (trig->type)
  {
    case TRIG_PUB:
      MAKE_STD_ZVAL(func);
      MAKE_STD_ZVAL(netw);
      MAKE_STD_ZVAL(nick);
      MAKE_STD_ZVAL(uhost);
      MAKE_STD_ZVAL(hand);
      MAKE_STD_ZVAL(chan);
      MAKE_STD_ZVAL(arg);

      ALLOC_INIT_ZVAL(ret);

      ZVAL_STRING(func, trig->command, 1);
      ZVAL_STRING(netw, net->label, 1);
      ZVAL_STRING(nick, data->prefix->nick, 1);
      ZVAL_STRING(uhost, data->prefix->host, 1);
      ZVAL_STRING(hand, data->prefix->nick, 1);
      ZVAL_STRING(chan, data->c_params[0], 1);
      ZVAL_STRING(arg, egg_makearg(data->rest_str,trig->mask), 1);

      php_args[0] = netw;
      php_args[1] = nick;
      php_args[2] = uhost;
      php_args[3] = hand;
      php_args[4] = chan;
      php_args[5] = arg;

      zend_try
      {
        if (call_user_function(CG(function_table), NULL, func, ret, 6, php_args TSRMLS_CC) != SUCCESS)
          troll_debug(LOG_WARN,"Error calling function\n");
        else
          FREE_ZVAL(ret);     
      } zend_end_try();

      if (netw)  FREE_ZVAL(netw);
      if (nick)  FREE_ZVAL(nick);
      if (uhost) FREE_ZVAL(uhost);
      if (hand)  FREE_ZVAL(hand);
      if (chan)  FREE_ZVAL(chan);
      if (arg)   FREE_ZVAL(arg);

      break;
    case TRIG_PUBM:
      MAKE_STD_ZVAL(func);
      MAKE_STD_ZVAL(netw);
      MAKE_STD_ZVAL(nick);
      MAKE_STD_ZVAL(uhost);
      MAKE_STD_ZVAL(hand);
      MAKE_STD_ZVAL(chan);
      MAKE_STD_ZVAL(arg);

      ALLOC_INIT_ZVAL(ret);

      ZVAL_STRING(func, trig->command, 1);
      ZVAL_STRING(netw, net->label, 1);
      ZVAL_STRING(nick, data->prefix->nick, 1);
      ZVAL_STRING(uhost, data->prefix->host, 1);
      ZVAL_STRING(hand, data->prefix->nick, 1);
      ZVAL_STRING(chan, data->c_params[0], 1);
      ZVAL_STRING(arg, data->rest_str, 1);

      php_args[0] = netw;
      php_args[1] = nick;
      php_args[2] = uhost;
      php_args[3] = hand;
      php_args[4] = chan;
      php_args[5] = arg;

      zend_try
      {
        if (call_user_function(CG(function_table), NULL, func, ret, 6, php_args TSRMLS_CC) != SUCCESS)
          troll_debug(LOG_WARN,"Error calling function\n");
        else
          FREE_ZVAL(ret);
      } zend_end_try();

      if (netw)  FREE_ZVAL(netw);
      if (nick)  FREE_ZVAL(nick);
      if (uhost) FREE_ZVAL(uhost);
      if (hand)  FREE_ZVAL(hand);
      if (chan)  FREE_ZVAL(chan);
      if (arg)   FREE_ZVAL(arg);

      break;
    case TRIG_MSG:
      MAKE_STD_ZVAL(func);
      MAKE_STD_ZVAL(netw);
      MAKE_STD_ZVAL(nick);
      MAKE_STD_ZVAL(uhost);
      MAKE_STD_ZVAL(hand);
      MAKE_STD_ZVAL(arg);

      ALLOC_INIT_ZVAL(ret);

      ZVAL_STRING(func, trig->command, 1);
      ZVAL_STRING(netw, net->label, 1);
      ZVAL_STRING(nick, data->prefix->nick, 1);
      ZVAL_STRING(uhost, data->prefix->host, 1);
      ZVAL_STRING(hand, data->prefix->nick, 1);
      ZVAL_STRING(arg, egg_makearg(data->rest_str,trig->mask), 1);

      php_args[0] = netw;
      php_args[1] = nick;
      php_args[2] = uhost;
      php_args[3] = hand;
      php_args[4] = arg;

      if (call_user_function(CG(function_table), NULL, func, ret, 5, php_args TSRMLS_CC) != SUCCESS)
        troll_debug(LOG_WARN,"Error calling function\n");
      else
        FREE_ZVAL(ret);

      if (netw)  FREE_ZVAL(netw);
      if (nick)  FREE_ZVAL(nick);
      if (uhost) FREE_ZVAL(uhost);
      if (hand)  FREE_ZVAL(hand);
      if (arg)   FREE_ZVAL(arg);

      break;
    case TRIG_MSGM:
      MAKE_STD_ZVAL(func);
      MAKE_STD_ZVAL(netw);
      MAKE_STD_ZVAL(nick);
      MAKE_STD_ZVAL(uhost);
      MAKE_STD_ZVAL(hand);
      MAKE_STD_ZVAL(arg);

      ALLOC_INIT_ZVAL(ret);

      ZVAL_STRING(func, trig->command, 1);
      ZVAL_STRING(netw, net->label, 1);
      ZVAL_STRING(nick, data->prefix->nick, 1);
      ZVAL_STRING(uhost, data->prefix->host, 1);
      ZVAL_STRING(hand, data->prefix->nick, 1);
      ZVAL_STRING(arg, data->rest_str, 1);

      php_args[0] = netw;
      php_args[1] = nick;
      php_args[2] = uhost;
      php_args[3] = hand;
      php_args[4] = arg;

      if (call_user_function(CG(function_table), NULL, func, ret, 5, php_args TSRMLS_CC) != SUCCESS)
        troll_debug(LOG_WARN,"Error calling function\n");
      else
        FREE_ZVAL(ret);

      if (netw)  FREE_ZVAL(netw);
      if (nick)  FREE_ZVAL(nick);
      if (uhost) FREE_ZVAL(uhost);
      if (hand)  FREE_ZVAL(hand);
      if (arg)   FREE_ZVAL(arg);

      break;
    case TRIG_JOIN:
      break;
    case TRIG_PART:
      break;
    case TRIG_SIGN:
      break;
  }
}

/* Not sure if this is needed */
static char* php_embed_read_cookies(TSRMLS_D)
{
  return NULL;
}

/* Again, probably not needed */
static int php_embed_deactivate(TSRMLS_D)
{
  /* fflush(stdout); */
  return SUCCESS;
}

/* In the case of echoing and other printing, this should go somewhere else
 * Cannot write to IRC directly due to multiple network setup, and shitty
 * PHP globalness
 */
static inline size_t php_embed_single_write(const char *str, uint str_length)
{
#ifdef PHP_WRITE_STDOUT
  long ret;

  ret = write(STDOUT_FILENO, str, str_length);
  if (ret <= 0) return 0;
  return ret;
#else
  size_t ret;

  ret = fwrite(str, 1, MIN(str_length, 16384), stdout);
  return ret;
#endif
}


/* See comment for above function */
static int php_embed_ub_write(const char *str, uint str_length TSRMLS_DC)
{
  const char *ptr = str;
  uint remaining = str_length;
  size_t ret;

  while (remaining > 0) 
  {
    ret = php_embed_single_write(ptr, remaining);
    if (!ret) 
    {
      php_handle_aborted_connection();
    }

    ptr += ret;
    remaining -= ret;
  }

  return str_length;
}

/* Probably not needed */
static void php_embed_flush(void *server_context)
{
  if (fflush(stdout) == EOF) 
  {
    php_handle_aborted_connection();
  }
}

/* Probably not needed, need to verify */
static void php_embed_send_header(sapi_header_struct *sapi_header, void *server_context TSRMLS_DC)
{
}


/* This should write to a special log */
static void php_embed_log_message(char *message)
{
  troll_debug(LOG_WARN, "%s\n", message);
}

/* Everything is really dealt with through functions, not really needed */
static void php_embed_register_variables(zval *track_vars_array TSRMLS_DC)
{
  php_import_environment_variables(track_vars_array TSRMLS_CC);
}

/* Initiate both the PHP module and the zend module */
static int php_embed_startup(sapi_module_struct *sapi_module)
{
  if (php_module_startup(sapi_module, NULL, 0)==FAILURE || zend_startup_module(&trollbot_module_entry) == FAILURE) {
    return FAILURE;
  }

  return SUCCESS;
}

sapi_module_struct php_embed_module = {
  "embed",                       /* name */
  "PHP Embedded Library",        /* pretty name */
 
  php_embed_startup,              /* startup */
  php_module_shutdown_wrapper,   /* shutdown */
  
  NULL,                          /* activate */
  php_embed_deactivate,           /* deactivate */
  
  php_embed_ub_write,             /* unbuffered write */
  php_embed_flush,                /* flush */
  NULL,                          /* get uid */
  NULL,                          /* getenv */
  
  php_error,                     /* error handler */
  
  NULL,                          /* header handler */
  NULL,                          /* send headers handler */
  php_embed_send_header,          /* send header handler */
	
  NULL,                          /* read POST data */
  php_embed_read_cookies,         /* read Cookies */
  
  php_embed_register_variables,   /* register server variables */
  php_embed_log_message,          /* Log message */
  NULL,							/* Get request time */
  
  STANDARD_SAPI_MODULE_PROPERTIES
};

int php_embed_init(int argc, char **argv PTSRMLS_DC)
{
  zend_llist global_vars;
#ifdef ZTS
  zend_compiler_globals *compiler_globals;
  zend_executor_globals *executor_globals;
  php_core_globals *core_globals;
  sapi_globals_struct *sapi_globals;
  void ***tsrm_ls;
#endif

#ifdef HAVE_SIGNAL_H
#if defined(SIGPIPE) && defined(SIG_IGN)
  signal(SIGPIPE, SIG_IGN); /* ignore SIGPIPE in standalone mode so
                               that sockets created via fsockopen()
			       don't kill PHP if the remote site
			       closes it.  in apache|apxs mode apache
			       does that for us!  thies@thieso.net
			       20000419 */
#endif
#endif

#ifdef PHP_WIN32
  _fmode = _O_BINARY;			/*sets default for file streams to binary */
  setmode(_fileno(stdin), O_BINARY);		/* make the stdio mode be binary */
  setmode(_fileno(stdout), O_BINARY);		/* make the stdio mode be binary */
  setmode(_fileno(stderr), O_BINARY);		/* make the stdio mode be binary */
#endif

#ifdef ZTS
  tsrm_startup(1, 1, 0, NULL);
#endif

#ifdef ZTS
  compiler_globals = ts_resource(compiler_globals_id);
  executor_globals = ts_resource(executor_globals_id);
  core_globals = ts_resource(core_globals_id);
  sapi_globals = ts_resource(sapi_globals_id);
  tsrm_ls = ts_resource(0);
  *ptsrm_ls = tsrm_ls;
#endif

  sapi_startup(&php_embed_module);

  if (php_embed_module.startup(&php_embed_module) == FAILURE) 
  {
    return FAILURE;
  }
 
  if (argv) 
  {
    php_embed_module.executable_location = argv[0];
  }

  zend_llist_init(&global_vars, sizeof(char *), NULL, 0);  

  /* Set some Embedded PHP defaults */
  SG(options) |= SAPI_OPTION_NO_CHDIR;
  zend_alter_ini_entry("log_errors",sizeof("log_errors"),"1", 1,PHP_INI_SYSTEM, PHP_INI_STAGE_RUNTIME);
  zend_alter_ini_entry("display_errors", 15, "1", 1, PHP_INI_SYSTEM, PHP_INI_STAGE_ACTIVATE);
  zend_alter_ini_entry("error_reporting", 16, "6143", 1, PHP_INI_SYSTEM, PHP_INI_STAGE_ACTIVATE); 
  zend_alter_ini_entry("register_argc_argv", 19, "1", 1, PHP_INI_SYSTEM, PHP_INI_STAGE_ACTIVATE);
  zend_alter_ini_entry("html_errors", 12, "0", 1, PHP_INI_SYSTEM, PHP_INI_STAGE_ACTIVATE);
  zend_alter_ini_entry("implicit_flush", 15, "1", 1, PHP_INI_SYSTEM, PHP_INI_STAGE_ACTIVATE);
  zend_alter_ini_entry("max_execution_time", 19, "0", 1, PHP_INI_SYSTEM, PHP_INI_STAGE_ACTIVATE);

  SG(request_info).argc=argc;
  SG(request_info).argv=argv;

  if (php_request_startup(TSRMLS_C) == FAILURE) 
  {
    php_module_shutdown(TSRMLS_C);
    return FAILURE;
  }
  
  SG(headers_sent) = 1;
  SG(request_info).no_headers = 1;
  php_register_variable("PHP_SELF", "-", NULL TSRMLS_CC);

  return SUCCESS;
}

/* Probably do not want this */
void php_embed_shutdown(TSRMLS_D)
{
  php_request_shutdown((void *) 0);
  php_module_shutdown(TSRMLS_C);
  sapi_shutdown();
#ifdef ZTS
  tsrm_shutdown();
#endif
}

static function_entry trollbot_functions[] = {
    PHP_FE(matchwild, NULL)
    PHP_FE(putserv, NULL)
    PHP_FE(bind, NULL)
    {NULL, NULL, NULL}
};

zend_module_entry trollbot_module_entry =
{
    STANDARD_MODULE_HEADER,
    "Trollbot",
    trollbot_functions,
    NULL, NULL, NULL, NULL, NULL,
    NO_VERSION_YET,
    STANDARD_MODULE_PROPERTIES,
};



