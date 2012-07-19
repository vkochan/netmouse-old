#!bash

MAKE=mingw32-make
MFLAGS=-C
SRCDIR=src
PROGRAM=netmouse.exe

rm $PROGRAM
rm $SRCDIR/$PROGRAM

$MAKE $MFLAGS $SRCDIR

cp $SRCDIR/$PROGRAM .
rm $SRCDIR/*.o
