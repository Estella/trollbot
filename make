#!/bin/sh

cd src
gcc -g -Wall -pedantic -std=c89 `for i in *.c; do echo "$i "; done;` -o ../test
cd ..
