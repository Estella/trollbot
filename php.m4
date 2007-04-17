dnl PHP finder thingy
dnl by poutine

AC_DEFUN([AX_PROG_PHP],[

  AC_ARG_WITH(php-config, [  --with-php=PATH        use php-config from PATH],
        [ with_php="$withval"])

  if test "x${with_php}" != "xno" ; then
    AC_MSG_CHECKING([for user specified php-config directory])

    if test -f "$with_php/php-config"; then
      PHPCONFIG="${with_php}/php-config"
      AC_MSG_RESULT([found])
    else
      AC_MSG_RESULT([Not specified])
      AC_PATH_PROG(PHPCONFIG,php-config)    
    fi

    AC_MSG_CHECKING([whether if we have proper compiler flags for PHP])

    if test "x${PHPCONFIG}" != "x"; then
      AC_MSG_RESULT([yes])

      PHP_CFLAGS=`${PHPCONFIG} --includes`

      php_ver="`${PHPCONFIG} --vernum`"

      if test $php_ver -gt 50000; then
        PHP_LDADD="`${PHPCONFIG} --ldflags` `${PHPCONFIG} --libs` -lphp5"
      else
        PHP_LDADD="`${PHPCONFIG} --ldflags` `${PHPCONFIG} --libs` -lphp4"
      fi
  
      dnl not sure if this is needed
      oldlibs="${LIBS}"
      oldcppflags="${CPPFLAGS}"
      CPPFLAGS="${oldcppflags} ${PHP_CFLAGS}"
      LIBS="${oldlibs} ${PHP_LDADD}"

dnl link      AC_MSG_CHECKING([if PHP cflags and ldflags work])

dnl link     AC_TRY_LINK([#include <main/php.h>
dnl link                  #include <main/SAPI.h>
dnl link                   #include <main/php_main.h>
dnl link                   #include <main/php_variables.h>
dnl link                   #include <main/php_ini.h>
dnl link                   #include <zend_ini.h>
dnl link                   ],[
dnl link                     const char *ap_auth_type(void) { return 0; }
dnl link                     const char *ap_loaded_modules(void) { return 0; }
dnl link                     const char *ap_log_rerror(void) { return 0; }
dnl link                     const char *ap_hook_post_config(void) { return 0; }
dnl link                     const char *apr_table_add(void) { return 0; }
dnl link                     const char *unixd_config(void) { return 0; }
dnl link                     const char *ap_get_brigade(void) { return 0; }
dnl link                     const char *ap_hook_handler(void) { return 0; }
dnl link                     const char *ap_update_mtime(void) { return 0; }
dnl link                     const char *apr_brigade_flatten(void) { return 0; }
dnl link                     const char *ap_add_cgi_vars(void) { return 0; }
dnl link                     const char *ap_server_root_relative(void) { return 0; }
dnl link                     const char *apr_table_set(void) { return 0; }
dnl link                     const char *ap_set_content_type(void) { return 0; }
dnl link                     const char *ap_server_root(void) { return 0; }
dnl link                     const char *ap_get_server_version(void) { return 0; }
dnl link                     const char *apr_pool_cleanup_register(void) { return 0; }
dnl link                     const char *ap_mpm_query(void) { return 0; }
dnl link                     const char *ap_destroy_sub_req(void) { return 0; }
dnl link                     const char *ap_pass_brigade(void) { return 0; }
dnl link                     const char *apr_pstrdup(void) { return 0; }
dnl link                     const char *apr_table_unset(void) { return 0; }
dnl link                     const char *apr_snprintf(void) { return 0; }
dnl link                     const char *ap_log_error(void) { return 0; }
dnl link                     const char *apr_table_get(void) { return 0; }
dnl link                     const char *ap_sub_req_lookup_uri(void) { return 0; }
dnl link                     const char *apr_psprintf(void) { return 0; }
dnl link                     const char *apr_pool_cleanup_run(void) { return 0; }
dnl link                     const char *ap_run_sub_req(void) { return 0; }
dnl link                     const char *apr_palloc(void) { return 0; }
dnl link                     const char *apr_brigade_cleanup(void) { return 0; }
dnl link                     const char *ap_hook_pre_config(void) { return 0; }
dnl link                     const char *ap_rwrite(void) { return 0; }
dnl link                     const char *apr_table_elts(void) { return 0; }
dnl link                     const char *ap_add_version_component(void) { return 0; }
dnl link                     const char *apr_bucket_eos_create(void) { return 0; }
dnl link                     const char *apr_pool_userdata_set(void) { return 0; }
dnl link                     const char *apr_brigade_create(void) { return 0; }
dnl link                     const char *ap_rflush(void) { return 0; }
dnl link                     const char *apr_pool_cleanup_null(void) { return 0; }
dnl link                     const char *ap_set_last_modified(void) { return 0; }
dnl link                     const char *ap_add_common_vars(void) { return 0; }
dnl link                     const char *apr_pool_userdata_get(void) { return 0; }
dnl link                   ],[
dnl link                     php_link="yes"
dnl link                   ],[
dnl link                     php_link="no"
dnl link                   ])

      LIBS="${oldlibs}"
      CPPFLAGS="${oldcppflags}"
                    
dnl link      if test "${php_link}" = "yes"; then
dnl link        AC_MSG_RESULT([linked fine])

        dnl for Automake
        AC_DEFINE([HAVE_PHP],1,[Ability to use PHP scripting])

        AC_SUBST(PHP_CFLAGS)
        AC_SUBST(PHP_LDADD)
dnl link      else
dnl link        AC_MSG_RESULT([could not link, PHP support disabled])
dnl link        PHP_CFLAGS=""
dnl link        PHP_LDADD=""

dnl link        AC_SUBST(PHP_CFLAGS)
dnl link        AC_SUBST(PHP_LDADD)
dnl link      fi
    else
      AC_MSG_RESULT([No])
    fi
  fi
] )
