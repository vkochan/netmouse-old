#!bash

MAKE=mingw32-make
MFLAGS=-C
SRCDIR=src

rm $SRCDIR/*.o

$MAKE $MFLAGS $SRCDIR 
