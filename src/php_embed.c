#include "../config.h"
#ifdef HAVE_PHP
/*
   Modified to make work with trollbot

 */
/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2004 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.0 of the PHP license,       |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_0.txt.                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Edin Kadribasic <edink@php.net>                              |
   +----------------------------------------------------------------------+
*/
/* $Id: php_embed.c,v 1.8.2.1 2005/01/25 22:00:14 andrei Exp $ */

#define ZEND_INCLUDE_FULL_WINDOWS_HEADERS

#include <php.h>
#include <ext/standard/php_smart_str.h>
#include <ext/standard/info.h>
#include <ext/standard/head.h>
#include <php_ini.h>
#include <SAPI.h>

#include "php_embed.h"
#include "main.h"
#include "servers.h"
#include "scripts.h"

#ifdef PHP_WIN32
#include <io.h>
#include <fcntl.h>
#endif

static int php_eval(struct irc_data *data, const char *line)
{
  char  *tmpbuf = NULL;
  int    size   = 0;

  TSRMLS_FETCH();

  if (zend_eval_string_ex(line, NULL, "Command line run code", 1 TSRMLS_CC) == FAILURE)
  {
    printf("Execution failed\n");
  }

  return 1;
}

static char* php_embed_read_cookies(TSRMLS_D)
{
	return NULL;
}

static int php_embed_deactivate(TSRMLS_D)
{
	fflush(stdout);
	return SUCCESS;
}

static inline size_t php_embed_single_write(const char *str, uint str_length)
{
  irc_printf(glob_server_head->sock,"%s\n",str);
}


static int php_embed_ub_write(const char *str, uint str_length TSRMLS_DC)
{
  irc_printf(glob_server_head->sock,"%s\n",str);
}

static void php_embed_flush(void *server_context)
{
	if (fflush(stdout)==EOF) {
		php_handle_aborted_connection();
	}
}

static void php_embed_send_header(sapi_header_struct *sapi_header, void *server_context TSRMLS_DC)
{
}

static void php_embed_log_message(char *message)
{
	fprintf (stderr, "%s\n", message);
}

static void php_embed_register_variables(zval *track_vars_array TSRMLS_DC)
{
	php_import_environment_variables(track_vars_array TSRMLS_CC);
}

static int php_embed_startup(sapi_module_struct *sapi_module)
{
	if (php_module_startup(sapi_module, NULL, 0)==FAILURE) {
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
  
	NULL,                     /* error handler */
  
	NULL,                          /* header handler */
	NULL,                          /* send headers handler */
	php_embed_send_header,          /* send header handler */
	
	NULL,                          /* read POST data */
	php_embed_read_cookies,         /* read Cookies */
  
	php_embed_register_variables,   /* register server variables */
	php_embed_log_message,          /* Log message */
  
	STANDARD_SAPI_MODULE_PROPERTIES
};
/* }}} */

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

  if (php_embed_module.startup(&php_embed_module)==FAILURE) {
	  return FAILURE;
  }
 
  if (argv) {
	php_embed_module.executable_location = argv[0];
  }

  zend_llist_init(&global_vars, sizeof(char *), NULL, 0);  

  /* Set some Embedded PHP defaults */
  SG(options) |= SAPI_OPTION_NO_CHDIR;
  zend_alter_ini_entry("register_argc_argv", 19, "1", 1, PHP_INI_SYSTEM, PHP_INI_STAGE_ACTIVATE);
  zend_alter_ini_entry("html_errors", 12, "0", 1, PHP_INI_SYSTEM, PHP_INI_STAGE_ACTIVATE);
  zend_alter_ini_entry("implicit_flush", 15, "1", 1, PHP_INI_SYSTEM, PHP_INI_STAGE_ACTIVATE);
  zend_alter_ini_entry("max_execution_time", 19, "0", 1, PHP_INI_SYSTEM, PHP_INI_STAGE_ACTIVATE);

  SG(request_info).argc=argc;
  SG(request_info).argv=argv;

  if (php_request_startup(TSRMLS_C)==FAILURE) {
	  php_module_shutdown(TSRMLS_C);
	  return FAILURE;
  }
  
  SG(headers_sent) = 1;
  SG(request_info).no_headers = 1;
  php_register_variable("PHP_SELF", "-", NULL TSRMLS_CC);

  return SUCCESS;
}

void php_embed_shutdown(TSRMLS_D)
{
	php_request_shutdown((void *) 0);
	php_module_shutdown(TSRMLS_C);
	sapi_shutdown();
#ifdef ZTS
    tsrm_shutdown();
#endif
}

int php_startup(void)
{
  struct scripts    *tmp = php_scripts_head;
  zend_file_handle  file_handle;

  TSRMLS_FETCH();

  /* Start up the interpreter */
  php_embed_init(0,NULL PTSRMLS_DC);

  while (tmp != NULL)
  {
    file_handle.type = ZEND_HANDLE_FILENAME;
    file_handle.filename = tmp->filename;
    file_handle.free_filename = 0;
    file_handle.opened_path = NULL;

    if (zend_execute_scripts(ZEND_REQUIRE TSRMLS_CC, NULL, 1, &file_handle) != SUCCESS) 
    {
      troll_debug(LOG_WARN,"PHP Script (%s) ran unsuccessfully",tmp->filename);
      tmp->status = 0;
    } else {
      printf("Successfully loaded %s\n",tmp->filename);
      tmp->status = 1;
    }

    tmp = tmp->next;
  }

  add_handler("php_eval",
              NULL,
              php_eval,
              NULL,
              1);

  add_trigger(PUB,
              "?php",
              "php_eval",
              NULL,
              "foo");


  return 1;
}

int php_handle(struct irc_data *data, const char *line)
{
  TSRMLS_FETCH();

  printf("We got called!\n");

  if (zend_eval_string_ex("echo \"I'm a midget!\";", NULL, "Command line run code", 1 TSRMLS_CC) == FAILURE)
  {
    printf("Execution failed\n");
  }

}

int php_shutdown(void)
{
  return 1;
}


/* PHP Functions YAY 
PHP_FUNCTION(putserv)
{
  zval **message;

  if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &message) == FAILURE) 
  {
    WRONG_PARAM_COUNT;
  }

  convert_to_string_ex(message);
 
  irc_printf(glob_server_head->sock,"%s\n",Z_STRVAL_PP(message));
}
*/



/*static function_entry trollbot_functions[] = {
        PHP_FE(putserv, NULL)
        {NULL, NULL, NULL}
};

zend_module_entry php_trollbot_module = {
        STANDARD_MODULE_HEADER,
        "trollbot",
        trollbot_functions,
        NULL, 
        NULL, 
        NULL,
        NULL,
        NULL,
        NO_VERSION_YET,
        STANDARD_MODULE_PROPERTIES
};*/

#endif /* HAVE_PHP */
