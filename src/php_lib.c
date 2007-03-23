/******************************
 * Trollbot                   *
 ******************************
 * Written by poutine DALnet  *
 ******************************
 * This software is public    *
 * domain. Free for any use   *
 * whatsoever.                *
 ******************************/
#include <php.h>
#include <ext/standard/php_smart_str.h"
#include <ext/standard/info.h"
#include <ext/standard/head.h"
#include <php_ini.h>
#include <SAPI.h>

#include "php_embed.h"
#include "main.h"
#include "servers.h"
#include "scripts.h"

PHP_MINIT_FUNCTION(sandwich)
{
  zend_class_entry trollbot;
  zend_class_entry trollbot_obj;
  zend_class_entry trollbot_sv;
  
  ZEND_INIT_MODULE_GLOBALS(php_trollbot_module, sw_initglobals, NULL);

  INIT_CLASS_ENTRY(trollbot, "Trollbot", trollbot_functions);
  trollbot_ce = zend_register_internal_class(&trollbot TSRMLS_CC); /* ?? */
  if (!trollbot_ce)  /* ?? */
  {
    return FAILURE;
  }

  INIT_CLASS_ENTRY(trollbot_obj, "TrollbotObject", trollbot_obj_functions);
  
  trollbot_obj.create_object = trollbot_obj_create_object;
  trollbot_obj_ce = zend_register_internal_class(&trollbot_obj TSRMLS_CC);
  if (!trollbot_obj_ce) 
  {
    return FAILURE;
  }

  memcpy(&trollbot_obj_handlers, zend_get_std_object_handlers(), 
      sizeof(trollbot_obj_handlers));

  trollbot_obj_handlers.read_dimension = sandwich_dim_read;
  trollbot_obj_handlers.write_dimension = sandwich_dim_write;
  trollbot_obj_handlers.get_method = sandwich_get_method;
  trollbot_obj_handlers.call_method = sandwich_call_method;

  INIT_CLASS_ENTRY(plsv, "PerlSV", plsv_functions);
  plsv_ce = zend_register_internal_class(&plsv TSRMLS_CC);
  plsv_ce->create_object = plsv_create_object;
  plsv_ce->get_iterator = plsv_iter_get;
  zend_class_implements(plsv_ce TSRMLS_CC, 1, zend_ce_traversable);
  if (!plsv_ce) {
    return FAILURE;
  }
  memcpy(&plsv_handlers, zend_get_std_object_handlers(), 
      sizeof(plsv_handlers));

  plsv_handlers.read_property = sv_prop_read;
  plsv_handlers.write_property = sv_prop_write;
  plsv_handlers.has_property = sv_prop_exists;
  plsv_handlers.get_method = sv_get_method;
  plsv_handlers.call_method = sv_call_method;
  plsv_handlers.get_class_name = sv_get_class_name;
  
  return SUCCESS;
}


static function_entry trollbot_functions[] = {
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
};

