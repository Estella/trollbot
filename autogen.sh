#!/bin/sh


# Prep
cat tcl.m4 > acinclude.m4
cat php.m4 >> acinclude.m4

autoheader
aclocal
autoconf
automake
autoheader
