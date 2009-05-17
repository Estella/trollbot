#!/bin/sh

./configure --with-tclsh=/usr/local/bin/tclsh8.4 --with-js=yes --with-perl --with-php=5 --with-libxml2 --with-xmpp --with-http --with-ics --prefix=/home/trollbot/hotgurl4u
for i in `find ./ -name Makefile`; do sed -i 's/O2/O0/g' $i; done;
