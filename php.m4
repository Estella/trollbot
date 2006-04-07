dnl PHP finder thingy

AC_DEFUN([AC_PROG_PHP], [

with_php=""
PHP_PREFIX=""
PHP_CFLAGS=""
PHP_LDADD=""

dnl check if provided --with-php=PATH
AC_ARG_WITH(php,      [  --with-php=PATH         use PHP from PATH],
	with_php="${withval}")

if test -r $with_php/include/php; then
    PHP_PREFIX="${with_php}"
    PHP_LDADD="-L${with_php}/lib -lphp5"
    PHP_CFLAGS="-I${with_php}/include/php/Zend -I${with_php}/include/php/TSRM \
                -I${with_php}/include/php/sapi -I${with_php}/include/php/main \
                -I${with_php}/include/php"
else
    echo "FOO ${with_php}"
fi

if test "x${PHP_PREFIX}" = "x"; then
    if test -r /usr/lib/libphp5.so; then
      PHP_PREFIX="/usr"
      PHP_LDADD="-L/usr/lib -lphp5"
      PHP_CFLAGS="-I/usr/include/php/Zend -I/usr/include/php/TSRM \
                  -I/usr/include/php/sapi -I/usr/include/php/main \
                  -I/usr/include/php"
    fi

    if test -r /usr/local/lib/libphp5.so; then
      PHP_PREFIX="/usr/local"
      PHP_LDADD="-L/usr/local/lib -lphp5"
      PHP_CFLAGS="-I/usr/local/include/php/Zend -I/usr/local/include/php/TSRM \
                  -I/usr/local/include/php/sapi -I/usr/local/include/php/main \
                  -I/usr/local/include/php"
    fi
fi
      
if test "x${PHP_PREFIX}" = "x"; then
    AC_MSG_WARN([could not find PHP! Disabling support and stealing your wife])
    with_php="no"
else
    AC_MSG_CHECKING( [Looking for PHP] )

    AC_MSG_RESULT( [Found at ${PHP_PREFIX}] )

    AC_MSG_CHECKING( [for PHP specific CFLAGS] )
    AC_MSG_RESULT( [${PHP_CFLAGS}] )

    AC_MSG_CHECKING( [for PHP specific LDFLAGS] )
    AC_MSG_RESULT( [${PHP_LDADD}] )

    dnl AC_DEFINE([HAVE_PHP],[],[for config.h])
fi

AC_SUBST(PHP_CFLAGS)
AC_SUBST(PHP_LDADD)
AC_SUBST(PHP_PREFIX)
] )
