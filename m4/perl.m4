dnl Perl finder thingy by poutine

AC_DEFUN([AX_PROG_PERL], [
  AC_ARG_WITH(perl,      [  --with-perl=PATH         use Perl from PATH],
        with_perl="$withval")

  if test "${with_perl}" != "no"; then
    AC_MSG_CHECKING([For perl binary])
    AC_PATH_PROG(PERL,perl)

    if test "x${PERL}" != "x"; then
      PERL_LDADD="`${PERL} -MExtUtils::Embed -e ldopts`"
      PERL_CFLAGS="`${PERL} -MExtUtils::Embed -e ccopts`"

      oldlibs="${LIBS}"
      oldcppflags="${CPPFLAGS}"
      CPPFLAGS="${oldcppflags} ${PERL_CFLAGS}"
      LIBS="${oldlibs} ${PERL_LDADD}"

      AC_MSG_CHECKING([whether we can link with perl])
      
      AC_TRY_LINK([#include <EXTERN.h>
                   #include <perl.h>
                  ],[
                   perl_alloc()
                  ],[
                   perl_link="yes"
                  ],[
                   perl_link="no"])

      LIBS="${oldlibs}"
      CPPFLAGS="${oldcppflags}"

      if test "x${perl_link}" = "xyes"; then
        AC_MSG_RESULT([Yes])
        AC_SUBST(PERL_LDADD)
        AC_SUBST(PERL_CFLAGS)
        AC_DEFINE([HAVE_PERL],1,[Ability to use Perl scripting])
      else
        PERL_LDADD=""
        PERL_CFLAGS=""
        AC_MSG_RESULT([No])
      fi
    fi
  fi
] )
