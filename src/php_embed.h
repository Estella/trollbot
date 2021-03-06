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
/* $Id: php_embed.h,v 1.6.2.2.2.1 2007/01/01 09:36:12 sebastian Exp $ */

#ifndef _PHP_EMBED_H_
#define _PHP_EMBED_H_


#ifdef HAVE_PHP
#include <main/php.h>
#include <main/SAPI.h>
#include <main/php_main.h>
#include <main/php_variables.h>
#include <main/php_ini.h>
#include <zend_ini.h>

/* HACK */
#ifdef HAVE_PERL
#undef END_EXTERN_C
#endif /* HAVE_PERL */
#endif /* HAVE_PHP */

#ifdef ZTS
#define PTSRMLS_D        void ****ptsrm_ls
#define PTSRMLS_DC       , PTSRMLS_D
#define PTSRMLS_C        &tsrm_ls
#define PTSRMLS_CC       , PTSRMLS_C

#define PHP_EMBED_START_BLOCK(x,y) { \
    void ***tsrm_ls; \
    php_embed_init(x, y PTSRMLS_CC); \
    zend_first_try {

#else
#define PTSRMLS_D
#define PTSRMLS_DC
#define PTSRMLS_C
#define PTSRMLS_CC

#define PHP_EMBED_START_BLOCK(x,y) { \
    php_embed_init(x, y); \
    zend_first_try {

#endif

#define PHP_EMBED_END_BLOCK() \
  } zend_catch { \
    /* int exit_status = EG(exit_status); */ \
  } zend_end_try(); \
  php_embed_shutdown(TSRMLS_C); \
}

struct t_timer;
struct trigger;
struct irc_data;
struct network;

extern zend_module_entry trollbot_module_entry;
#define phpext_trollbot_ptr &trollbot_module_entry

void t_timer_php_handler(struct network *net, struct t_timer *timer);
void php_load_scripts_from_config(struct config *cfg);
void myphp_eval_file(char *filename);
void php_handler(struct network *net, struct trigger *trig, struct irc_data *data, struct dcc_session *dcc, const char *dccbuf);
int php_embed_init(int argc, char **argv PTSRMLS_DC);
void php_embed_shutdown(TSRMLS_D);

extern sapi_module_struct php_embed_module;


#endif /* _PHP_EMBED_H_ */
