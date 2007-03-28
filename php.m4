dnl PHP finder thingy

AC_DEFUN([AC_PROG_PHP], [
  AC_ARG_WITH(php,      [  --with-php=PATH         use PHP from PATH],
        with_php="$withval")

  AC_MSG_CHECKING([Checking whether to use PHP])

  if test "$with_php" != "no" ; then
    if test -r $with_php/include/php ; then
      PHP_PREFIX="${with_php}"
      PHP_LDADD="${with_php}/lib/libphp5.so -R${with_php}/lib `${with_php}/bin/php-config --libs`"
      PHP_CFLAGS="`${with_php}/bin/php-config --includes`"
    fi

    if test "x${PHP_PREFIX}" = "x"; then
      if test -r /usr/lib/libphp5.so; then
        PHP_PREFIX="/usr"
        PHP_LDADD="/usr/lib/libphp5.so `/usr/bin/php-config --libs`"
        PHP_CFLAGS="`/usr/bin/php-config --includes`"
      fi

      if test -r /usr/local/lib/libphp5.so; then
        PHP_PREFIX="/usr/local"
        PHP_LDADD="/usr/local/lib/libphp5.so `/usr/local/bin/php-config --libs`"
        PHP_CFLAGS="`/usr/local/bin/php-config --includes`"
      fi
    fi

    if test "x${PHP_CFLAGS}" != "x" -a "x${PHP_LDADD}" != "x" ; then
      AC_DEFINE([HAVE_PHP],1,[Ability to use PHP scripting])

      AC_SUBST(PHP_CFLAGS)
      AC_SUBST(PHP_LDADD)
      AC_SUBST(PHP_PREFIX)

      AC_MSG_RESULT([ready to go])
    else
      AC_MSG_RESULT([Could not use PHP])
    fi
  else
    AC_MSG_RESULT([support disabled])
  fi
] )
