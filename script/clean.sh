#!/bin/sh

SUCCESS="\033[32mSuccess:\033[00m"

# Enter masterproject directory
MY_PATH=`dirname "$0"`
cd $MY_PATH/..

# Remove folders and contents
rm -rf build
rm -rf lib
rm -rf bin/Debug
rm -rf bin/Release

echo -e $SUCCESS "clean complete"
