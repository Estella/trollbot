#!/bin/sh


# Prep
cat tcl.m4 > acinclude.m4
cat php.m4 >> acinclude.m4
cat perl.m4 >> acinclude.m4
cat python.m4 >> acinclude.m4

aclocal
autoconf
autoheader
automake
