#!/bin/bash

# Determine path to script
MY_PATH=`dirname "$0"`

cd $MY_PATH/..

# Build and run target
ninja -C build/Debug masterproject_"$1" && cd bin/"$1" && Debug/masterproject_"$1"
