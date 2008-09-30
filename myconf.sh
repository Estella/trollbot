#!/bin/sh

CXXFLAGS="-g -O0"
./configure --prefix=/home/ben/Projects/trollbot --with-tclsh=/usr/bin/tclsh8.4 --with-js=yes --with-python --with-perl --with-php=5 --with-libxml2 --with-ics --with-xmpp --with-httpd

for i in `find ./ -name Makefile`; do sed -i 's/O2/O0/g' $i; done;
