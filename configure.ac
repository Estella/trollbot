VERSION = "1.0.0"

AC_SUBST(VERSION)

dnl set the default prefix to be /home/user/trollbot

AC_INIT(src/main.c)
AM_INIT_AUTOMAKE(trollbot,1.0.0)

AC_CONFIG_HEADER(config.h)

AM_MAINTAINER_MODE

AC_PROG_CC

dnl test C Compiler
AC_LANG_C

AC_C_CONST

AC_STRUCT_TM

AC_CHECK_HEADERS([time.h])
AC_CHECK_HEADERS([sys/time.h])

dnl set HAVE_WORKING_FORK if system supports forking
AC_FUNC_FORK

AC_TYPE_PID_T

dnl test if TCL exists
AX_PROG_TCL

dnl set the WITH_TCL for automake's use
AM_CONDITIONAL(WITH_TCL, test "x${TCL_LIBS}" != "x")

dnl test if PHP exists
AX_PROG_PHP

dnl supply automake a WITH_PHP conditional
AM_CONDITIONAL(WITH_PHP, test "x${PHP_LDADD}" != "x")

AX_PROG_PERL

AM_CONDITIONAL(WITH_PERL, test "x${PERL_LDADD}" != "x")

dnl AC_PYTHON_DEVEL

AM_CONDITIONAL(WITH_PYTHON, test "x${PYTHON_LDADD}" != "x")

AC_PROG_MAKE_SET

AC_HEADER_STDC

dnl Set the prefix to /home/trollbot if none exists
if test $prefix = NONE; then
    prefix=${HOME}/trollbot
fi

AC_OUTPUT(Makefile 
          src/Makefile
          doc_dist/Makefile
          scripts_dist/Makefile
          conf_dist/Makefile)



echo ""
echo "  ********** Trollbot Configuration **********  "

if test "x${TCL_LIBS}" != "x"; then
  echo "  TCL    Support: Yes"             
else
  echo "  TCL    Support: No"
fi

if test "x${PHP_LDADD}" != "x"; then
  echo "  PHP    Support: Yes"
else
  echo "  PHP    Support: No"
fi

if test "x${PERL_LDADD}" != "x"; then
  echo "  Perl   Support: Yes"
else
  echo "  Perl   Support: No"
fi

if test "x${PYTHON_LDADD}" != "x"; then
  echo "  Python Support: Yes"
else
  echo "  Python Support: No"
fi

echo ""
echo "  Files will be installed to: $prefix "
echo "  Type 'make' then 'make install' to continue"
echo ""
