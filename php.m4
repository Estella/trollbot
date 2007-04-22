dnl PHP finder thingy
dnl by poutine

AC_DEFUN([AX_PROG_PHP],[
  AC_ARG_WITH(php-cflags, [  --with-php-cflags=FLAGS  CFLAGS to use for PHP],
        [ with_php_cflags="$withval" ])
  AC_ARG_WITH(php-ldflags,[  --with-php-ldflags=FLAGS LDFLAGS to use PHP],
        [ with_php_ldflags="$withval" ])
  AC_ARG_WITH(php-config, [  --with-php-config=PATH   use php-config from PATH],
        [ with_php_config="$withval" ] )             
  AC_ARG_WITH(php-lib,    [  --with-php-lib=PATH      PATH to libphp5.so or libphp4.so],
        [ with_php_lib="$withval" ] )
  AC_ARG_WITH(php,        [  --with-php=(4|5)         configure with PHP4 or 5 support],
        [ with_php="$withval" ] )

  dnl Default to check for PHP
  if test "x${with_php}" != "xno" ; then
    AC_MSG_CHECKING([if there's a valid user specified php-config])

    if test -f "${with_php_config}/php-config"; then
      PHPCONFIG="${with_php_config}/php-config"
      AC_MSG_RESULT([Found at ${PHPCONFIG}])
    else
      AC_MSG_RESULT([Not specified, checking elsewhere])
      AC_PATH_PROG(PHPCONFIG,php-config)
    fi

    if test "x${PHPCONFIG}" != "x"; then
      AC_MSG_CHECKING([for CFLAGS to use with PHP])
    
      if test "x${with_php_cflags}" != "x"; then
        PHP_CFLAGS="${with_php_cflags} `${PHPCONFIG} --includes`"
      else
        PHP_CFLAGS="`${PHPCONFIG} --includes`"
      fi
   
      AC_MSG_RESULT([${PHP_CFLAGS}])

      AC_MSG_CHECKING([whether our test PHP program compiles])
  
      oldcppflags="${CPPFLAGS}"

      CPPFLAGS="${oldcppflags} ${PHP_CFLAGS}"

      AC_TRY_COMPILE([#include <main/php.h>
                      #include <main/SAPI.h>
                      #include <main/php_main.h>
                      #include <main/php_variables.h>
                      #include <main/php_ini.h>
                      #include <zend_ini.h>
                      ],[
                        zval *test;
                      ],[
                        php_compile="yes"
                      ],[
                        php_compile="no"
                      ])
 
      if test "${php_compile}" = "yes"; then
        AC_MSG_RESULT([Wonderbar])
        dnl yay we compiled, now to test linking

        if test "${with_php}" = "5" || test "x${with_php}" = "x"; then 
          AC_MSG_CHECKING([for LDFLAGS to use with PHP5])
        
          if test "x${with_php_ldflags}" != "x"; then
            PHP_LDADD="${with_php_ldflags} `${PHPCONFIG} --libs` `${PHPCONFIG} --ldflags` -lphp5 -Wl,--unresolved-symbols=ignore-all"
          else
            PHP_LDADD="`${PHPCONFIG} --libs` `${PHPCONFIG} --ldflags` -lphp5 -Wl,--unresolved-symbols=ignore-all"
          fi

          if test "x${with_php_lib}" != "x"; then
            PHP_LDADD="-L${with_php_lib} -Wl,-rpath=${with_php_lib} ${PHP_LDADD}"
          fi

          AC_MSG_RESULT([${PHP_LDADD}])

          AC_MSG_CHECKING([whether our test PHP5 program can link])
          
          oldlibs="${LIBS}"
          LIBS="${oldlibs} ${PHP_LDADD}"

          AC_TRY_LINK([#include <main/php.h>
                       #include <main/SAPI.h>
                       #include <main/php_main.h>
                       #include <main/php_variables.h>
                       #include <main/php_ini.h>
                       #include <zend_ini.h>
                       ],[
                         zval *test;
                       ],[
                         php_link="yes"
                       ],[
                         php_link="no"
                       ])

          LIBS="${oldlibs}"

          if test "${php_link}" = "yes"; then
            AC_MSG_RESULT([Wonderbar])
            AC_DEFINE([HAVE_PHP],1,[Ability to use PHP scripting])

            AC_SUBST(PHP_CFLAGS)
            AC_SUBST(PHP_LDADD)
          else
            AC_MSG_RESULT([Error, cannot use PHP5])
          fi
        fi

        if test "${with_php}" = "4" || test "${php_link}" = "no"; then
          AC_MSG_CHECKING([for LDFLAGS to use with PHP4])

          if test "x${with_php_ldflags}" != "x"; then
            PHP_LDADD="${with_php_ldflags} `${PHPCONFIG} --libs` `${PHPCONFIG} --ldflags` -lphp4 -Wl,--unresolved-symbols=ignore-all"
          else
            PHP_LDADD="`${PHPCONFIG} --libs` `${PHPCONFIG} --ldflags` -lphp4 -Wl,--unresolved-symbols=ignore-all"
          fi

          if test "x${with_php_lib}" != "x"; then
            PHP_LDADD="-L${with_php_lib} -Wl,-rpath=${with_php_lib} ${PHP_LDADD}"
          fi

          AC_MSG_RESULT([${PHP_LDADD}])

          AC_MSG_CHECKING([whether our test PHP4 program can link])

          oldlibs="${LIBS}"
          LIBS="${oldlibs} ${PHP_LDADD}"

          AC_TRY_LINK([#include <main/php.h>
                       #include <main/SAPI.h>
                       #include <main/php_main.h>
                       #include <main/php_variables.h>
                       #include <main/php_ini.h>
                       #include <zend_ini.h>
                       ],[
                         zval *test;
                       ],[
                         php_link="yes"
                       ],[
                         php_link="no"
                       ])

          LIBS="${oldlibs}"

          if test "${php_link}" = "yes"; then
            AC_MSG_RESULT([Wonderbar])
            AC_DEFINE([HAVE_PHP],1,[Ability to use PHP scripting])

            AC_SUBST(PHP_CFLAGS)
            AC_SUBST(PHP_LDADD)
          else
            AC_MSG_RESULT([Error, cannot use PHP4])
          fi

        fi
      else        
        AC_MSG_RESULT([Did not compile. PHP disabled])
      fi
  
      CPPFLAGS="${oldcppflags}"
    else
      AC_MSG_WARN([No php-config found or specified, PHP disabled.])
    fi
  else
    AC_MSG_WARN([PHP Support disabled])
  fi      
] )
