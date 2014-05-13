#!/bin/bash

# Determine path to script
MY_PATH=`dirname "$0"`

cd $MY_PATH/..

# Build and run target
ninja -C build/Debug "$1" && cd bin && valgrind --tool=memcheck --leak-check=yes --log-file=../build/Debug/"$1".leaklog Debug/"$1"
