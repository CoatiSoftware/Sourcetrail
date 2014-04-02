#!/bin/bash

# Determine path to script
MY_PATH=`dirname "$0"`

cd $MY_PATH/..

# Build and run target
make -C build/Release "$1" && cd bin && Release/"$1"
