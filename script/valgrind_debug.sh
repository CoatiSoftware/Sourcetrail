#!/bin/sh

# Determine path to script
MY_PATH=`dirname "$0"`

cd $MY_PATH/..

# Build and run target
make -C build/Debug "$1" && valgrind --tool=memcheck --leak-check=yes --log-file=build/Debug/"$1".leaklog bin/Debug/"$1"
