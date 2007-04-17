dnl FIRST PASS AFTER STEALING THIS FROM CYRUS!
dnl USE AT YOUR OWN PERIL!
dnl I MEAN IT!
dnl
dnl tcl.m4: an autoconf Tcl locator
dnl $Id: tcl.m4,v 1.5 2005/03/05 00:36:14 dasenbro Exp $
dnl
dnl This is rob's Tcl macro, fixed by tjs.  It may need occasional tweaking,
dnl but until the next impediment to compilation, it's fill-in-the-blank,
dnl and it should be able to do reasonable things with user input.
dnl
dnl This will probably just work on Andrew systems, but given the variety
dnl and apparent creativity of people who compile Tcl elsewhere, I don't know
dnl what it will do.  I have yet to see an autoconf Tcl test that users were
dnl happy with.
dnl
dnl BUGS
dnl   The command-line arguments are overcomplicated.
dnl   There are doubtlessly others...

dnl To use this macro, just do CMU_TCL.  It outputs
dnl TCL_LIBS, TCL_CPPFLAGS, and TCL_DEFS and SUBSTs them.  
dnl If successful, these have stuff in them.  If not, they're empty.
dnl If not successful, with_tcl has the value "no".

AC_DEFUN([AX_PROG_TCL], [
# --- BEGIN CMU_TCL ---
dnl To link against Tcl, configure does several things to make my life
dnl "easier".
dnl
dnl * maybe ask the user where they think Tcl lives, and try to find it
dnl * maybe ask the user what "tclsh" is called this week (i.e., "tclsh8.0")
dnl * run tclsh, ask it for a path, then run that path through sed
dnl * sanity check its result (many installs are a little broken)
dnl * try to figure out where Tcl is based on this result
dnl * try to guess where the Tcl include files are
dnl
dnl Notes from previous incarnations:
dnl > XXX MUST CHECK FOR TCL BEFORE KERBEROS V4 XXX
dnl > This is because some genius at MIT named one of the Kerberos v4
dnl > library functions log().  This of course conflicts with the
dnl > logarithm function in the standard math library, used by Tcl.
dnl
dnl > Checking for Tcl first puts -lm before -lkrb on the library list.
dnl

dnl Check for some information from the user on what the world looks like
AC_ARG_WITH(tclconfig,[  --with-tclconfig=PATH   use tclConfig.sh from PATH
                          (configure gets Tcl configuration from here)],
        dnl trim tclConfig.sh off the end so we can add it back on later.
	TclLibBase=`echo ${withval} | sed s/tclConfig.sh\$//`)
AC_ARG_WITH(tcl,      [  --with-tcl=PATH         use Tcl from PATH],
	TclLibBase="${withval}/lib")
AC_ARG_WITH(tclsh,    [  --with-tclsh=TCLSH      use TCLSH as the tclsh program
                          (let configure find Tcl using this program)],
	TCLSH="${withval}")

if test "$TCLSH" = "no" -o "$with_tclconfig" = "no" ; then
  AC_MSG_WARN([Tcl disabled because tclsh or tclconfig specified as "no"])
  with_tcl=no
fi

if test "$with_tcl" != "no"; then
  if test \! -z "$with_tclconfig" -a \! -d "$with_tclconfig" ; then
    AC_MSG_ERROR([--with-tclconfig requires a directory argument.])
  fi

  if test \! -z "$TCLSH" -a \! -x "$TCLSH" ; then
    AC_MSG_ERROR([--with-tclsh must specify an executable file.])
  fi

  if test -z "$TclLibBase"; then # do we already know?
    # No? Run tclsh and ask it where it lives.

    # Do we know where a tclsh lives?
    if test -z "$TCLSH"; then
      # Try and find tclsh.  Any tclsh.
      # If a new version of tcl comes out and unfortunately adds another
      # filename, it should be safe to add it (to the front of the line --
      # somef vendors have older, badly installed tclshs that we want to avoid
      # if we can)
      AC_PATH_PROGS(TCLSH, [tclsh8.4 tclsh8.3 tclsh8.2 tclsh8.1 tclsh8.0 tclsh], "unknown")
    fi

    # Do we know where to get a tclsh?
    if test "${TCLSH}" != "unknown"; then
      AC_MSG_CHECKING([where Tcl says it lives])
      TclLibBase=`echo puts \\\$tcl_library | ${TCLSH} | sed -e 's,[^/]*$,,'`
      AC_MSG_RESULT($TclLibBase)
    fi
  fi

  if test -z "$TclLibBase" ; then
    AC_MSG_RESULT([can't find tclsh])
    AC_MSG_WARN([can't find Tcl installtion; use of Tcl disabled.])
    with_tcl=no
  else
    AC_MSG_CHECKING([for tclConfig.sh])
    # Check a list of places where the tclConfig.sh file might be.
    for tcldir in "${TclLibBase}" \
                  "${TclLibBase}/.." \
		  "${TclLibBase}"`echo ${TCLSH} | sed s/sh//` ; do
      if test -f "${tcldir}/tclConfig.sh"; then
        TclLibBase="${tcldir}"
        break
      fi
    done

    if test -z "${TclLibBase}" ; then
      AC_MSG_RESULT("unknown")
      AC_MSG_WARN([can't find Tcl configuration; use of Tcl disabled.])
      with_tcl=no
    else
      AC_MSG_RESULT(${TclLibBase}/)
    fi

    if test "${with_tcl}" != no ; then
      AC_MSG_CHECKING([Tcl configuration on what Tcl needs to compile])
      if test -f ${TclLibBase}/tclConfig.sh; then 
        AC_MSG_RESULT([ok])
        . ${TclLibBase}/tclConfig.sh
      else
        AC_MSG_RESULT(not found)
        with_tcl=no
      fi
    fi

    if test "${with_tcl}" != no ; then
      dnl Now, hunt for the Tcl include files, since we don't strictly
      dnl know where they are; some folks put them (properly) in the 
      dnl default include path, or maybe in /usr/local; the *BSD folks
      dnl put them in other places.
      AC_MSG_CHECKING([where Tcl includes are])
      for tclinclude in "${TCL_PREFIX}/include/tcl${TCL_VERSION}" \
                        "${TCL_PREFIX}/include/tcl" \
                        "${TCL_PREFIX}/include" ; do
        if test -r "${tclinclude}/tcl.h" ; then
          TCL_CPPFLAGS="-I${tclinclude}"
          break
        fi
      done
      if test -z "${TCL_CPPFLAGS}" ; then
        AC_MSG_WARN(can't find Tcl includes; use of Tcl disabled.)
        with_tcl=no
      fi
      AC_MSG_RESULT(${TCL_CPPFLAGS})
    fi
    
    # Finally, pick up the Tcl configuration if we haven't found an
    # excuse not to.
    if test "${with_tcl}" != no; then
      dnl TCL_LIBS="${TCL_LD_SEARCH_FLAGS} ${TCL_LIB_SPEC}"
      TCL_LIBS="${TCL_LIB_SPEC} ${TCL_LIBS}"
    fi

    if test "x${TCL_CPPFLAGS}" != "x" -a "x${TCL_LIBS}" != "x" ; then
      AC_DEFINE(HAVE_TCL, 1, [Ability to use TCL scripting])
    fi
  fi
fi

AC_SUBST(TCL_DEFS)
AC_SUBST(TCL_LIBS)
AC_SUBST(TCL_CPPFLAGS)

# --- END CMU_TCL ---
]) dnl AX_PROG_TCL
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
        AC_MSG_RESULT([No])
      fi
    fi
  fi
] )
