#!/bin/sh


# Prep
cat m4/tcl.m4 > acinclude.m4
cat m4/php.m4 >> acinclude.m4
cat m4/perl.m4 >> acinclude.m4
cat m4/python.m4 >> acinclude.m4

aclocal
autoconf
autoheader
automake
