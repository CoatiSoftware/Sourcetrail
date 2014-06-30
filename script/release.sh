#!/bin/bash

# Determine path to script
MY_PATH=`dirname "$0"`

cd $MY_PATH/..

# Build and run target
ninja -C build/Release masterproject_"$1" && cd bin/"$1" && Release/masterproject_"$1"
