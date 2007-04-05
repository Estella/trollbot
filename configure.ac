VERSION = "0.0.1"
AC_SUBST(VERSION)

dnl set the default prefix to be /home/user/trollbot

AC_INIT(src/main.c)
AM_INIT_AUTOMAKE(trollbot,0.0.1)

AC_CONFIG_HEADER(config.h)

AC_PROG_CC

dnl test C Compiler
AC_LANG_C

dnl test if TCL exists
CMU_TCL

dnl set the WITH_TCL for automake's use
AM_CONDITIONAL(WITH_TCL, test "x${TCL_LIBS}" != "x")

dnl test if PHP exists
AC_PROG_PHP

dnl supply automake a WITH_PHP conditional
AM_CONDITIONAL(WITH_PHP, test "x${PHP_LDADD}" != "x")

AC_PROG_MAKE_SET

AC_HEADER_STDC

AC_OUTPUT(Makefile 
          src/Makefile)


