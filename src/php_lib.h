#ifndef __PHP_LIB_H__
#define __PHP_LIB_H__

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

PHP_FUNCTION(ispermban);
PHP_FUNCTION(matchattr);
PHP_FUNCTION(isbansticky);
PHP_FUNCTION(isban);
PHP_FUNCTION(unbind);
PHP_FUNCTION(utimer);
PHP_FUNCTION(botnick);
PHP_FUNCTION(chhandle);
PHP_FUNCTION(passwdok);
PHP_FUNCTION(save);
PHP_FUNCTION(finduser);
PHP_FUNCTION(countusers);
PHP_FUNCTION(savechannels);
PHP_FUNCTION(validuser);
PHP_FUNCTION(countusers);
PHP_FUNCTION(putdcc);
PHP_FUNCTION(matchwild);
PHP_FUNCTION(bind);
PHP_FUNCTION(putserv);

#endif /* __PHP_LIB_H__ */
