dnl m4 script to find tcl
dnl does NOT use tclConfig.sh (due to the general shittiness of that mechanism)
dnl poutine/DALnet
dnl This file is part of trollbot and it is public domain

AC_DEFUN([AC_PROG_TCL], [

without_tcl=""
with_tcl_inc=""
with_tcl_lib=""
with_tcl_cflags=""
with_tcl_ldflags=""

TCL_INC=""
TCL_LIB=""

TCL_CFLAGS=""
TCL_LDFLAGS=""

TCL_VERSION_MAJ=""
TCL_VERSION_MIN=""

dnl check if provided --with-tcl-inc=PATH
AC_ARG_WITH(tcl-inc,      [  --with-tcl-inc=PATH         Path to tcl.h],
	[with_tcl_inc=$withval])

AC_ARG_WITH(tcl-lib,      [  --with-tcl-lib=PATH         Path to libtcl8.x.so],
	[with_tcl_lib=$withval])

AC_ARG_WITH(tcl-cflags,      [  --with-tcl-cflags=CFLAGS    CFLAG override for Tcl],
	[with_tcl_cflags=$withval])

AC_ARG_WITH(tcl-ldflags,      [  --with-tcl-ldflags=LDFLAGS  LDFLAGS override for Tcl],
	[with_tcl_ldflags=$withval])


dnl not sure if this is how I'm supposed to do it
AC_ARG_WITH(without_tcl,      [ --without-tcl                Compile without TCL support],
	[without_tcl=no])

if test "x${without_tcl}" = "x" ; then
    dnl first check if they provided args, and if they are correct
    if test "x${with_tcl_inc}" = "x"; then
        echo "${with_tcl_inc}"
        AC_MSG_CHECKING([for tcl.h in $with_tcl_inc])
        if test -r "${with_tcl_inc}/tcl.h" ; then
            TCL_INC=${with_tcl_inc}
            AC_MSG_RESULT([found])
        else
            AC_MSG_RESULT([not found])
        fi
    fi
  
    if test -r "${with_tcl_lib}" ; then
        AC_MSG_CHECKING([for tcl??.so or .dll])
        if test -r "${with_tcl_lib}/libtcl??.so" ; then
            TCL_LIB=${with_tcl_lib}
            AC_MSG_RESULT([found])
        else
            dnl cygwin
            if test -r "${with_tcl_lib}/libtcl??.dll" ; then
                TCL_LIB=${with_tcl_lib}
                AC_MSG_RESULT([found])
            else
                AC_MSG_RESULT([not found])
            fi
        fi
    fi

    dnl Finished checking user provided args
    dnl if they provided them, and they were correct
    dnl TCL_LIB and/or TCL_INC should be set

    if test "x${TCL_INC}" = "x" ; then
        AC_MSG_CHECKING([for tcl.h in /usr/include])
        if test -r "/usr/include/tcl.h" ; then
            AC_MSG_RESULT([found])
            TCL_INC="/usr/include"
        else
            AC_MSG_RESULT([not found])
            AC_MSG_CHECKING([for tcl.h in /usr/local/include])
            if test -r "/usr/local/include/tcl.h" ; then
                AC_MSG_RESULT([found])
                TCL_INC="/usr/local/include"
            else
                AC_MSG_RESULT([not found])
            fi
        fi
    fi
   
    if test "x${TCL_LIB}" = "x" ; then
        if test -r "/usr/lib/libtcl??.so" ; then
            TCL_LIB="/usr/lib"
        fi

        dnl cygwin
        if test -r "/usr/lib/libtcl??.dll" ; then
            TCL_LIB="/usr/lib"
        fi

        if test -r "/usr/local/lib/libtcl??.so" ; then
            TCL_LIB="/usr/local/lib"
        fi

        dnl cygwin
        if test -r "/usr/local/lib/libtcl??.dll" ; then
            TCL_LIB="/usr/local/lib"
        fi
    fi

    dnl TCL_INC and TCL_LIB should be set if we can use TCL right now
    AC_MSG_CHECKING([for the usability of tcl.h])
    if test "x${TCL_INC}" != "x" ; then
        AC_MSG_RESULT([Yes])
        AC_MSG_CHECKING([for the usability of libtcl??.so or libtcl??.dll])
        if test "x${TCL_LIB}" != "x" ; then
            AC_MSG_RESULT( [Yes] )
        else
            AC_MSG_RESULT( [No])
        fi
    else
        AC_MSG_RESULT([Not found])
    fi
    
fi

AC_SUBST(TCL_CFLAGS)
AC_SUBST(TCL_LDFLAGS)
] )
