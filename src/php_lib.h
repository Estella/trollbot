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

PHP_FUNCTION(countusers);
PHP_FUNCTION(putdcc);
PHP_FUNCTION(matchwild);
PHP_FUNCTION(bind);
PHP_FUNCTION(putserv);

#endif /* __PHP_LIB_H__ */
