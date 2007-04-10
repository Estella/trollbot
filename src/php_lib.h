#ifndef __PHP_LIB_H__
#define __PHP_LIB_H__

#include <main/php.h>
#include <main/SAPI.h>
#include <main/php_main.h>
#include <main/php_variables.h>
#include <main/php_ini.h>
#include <zend_ini.h>
#undef END_EXTERN_C

PHP_FUNCTION(matchwild);
PHP_FUNCTION(bind);
PHP_FUNCTION(putserv);

#endif /* __PHP_LIB_H__ */
