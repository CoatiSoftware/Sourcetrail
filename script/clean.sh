#!/bin/bash

SUCCESS="\033[32mSuccess:\033[00m"

# Determine current platform
PLATFORM='unknown'
if [ "$(uname)" == "Darwin" ]; then
	PLATFORM='MacOS'
elif [ "$(expr substr $(uname -s) 1 5)" == "Linux" ]; then
	PLATFORM='Linux'
elif [ "$(expr substr $(uname -s) 1 10)" == "MINGW32_NT" ]; then
	PLATFORM='Windows'
fi


if [ $PLATFORM == "Windows" ]; then
	ORIGINAL_PATH_TO_SCRIPT="${0}"
	CLEANED_PATH_TO_SCRIPT="${ORIGINAL_PATH_TO_SCRIPT//\\//}"
	ROOT_DIR=`dirname "$CLEANED_PATH_TO_SCRIPT"`
else
	ROOT_DIR="$( cd "$( dirname "$0" )" && pwd )"
fi

ROOT_DIR=$ROOT_DIR/..

# Enter masterproject directory
cd $ROOT_DIR/

if [ $PLATFORM == "Windows" ]; then
	BACKSLASHED_ROOT_DIR="${ROOT_DIR//\//\\}"
    # Remove symbolic links
    cmd //c 'rmdir '$BACKSLASHED_ROOT_DIR'\bin\app\Debug\data' &
    cmd //c 'rmdir '$BACKSLASHED_ROOT_DIR'\bin\app\Debug\user' &
    cmd //c 'rmdir '$BACKSLASHED_ROOT_DIR'\bin\app\Release\data' &
    cmd //c 'rmdir '$BACKSLASHED_ROOT_DIR'\bin\app\Release\user' &
fi

# Remove folders and contents
rm -rf build
rm -rf bin/app/Debug
rm -rf bin/app/Release
rm -rf bin/lib/Debug
rm -rf bin/lib/Release
rm -rf bin/test/Debug
rm -rf bin/test/Release
rm -rf bin/gen
rm -rf bin/lib
rm -rf bin/lib_gui
rm -rf bin/lib_license
rm -rf bin/lib_parser
rm -rf bin/license_generator

# Remove log files
find bin/test/data/log -type f -name 'log*' -delete

echo -e $SUCCESS "clean complete"
