#!/bin/bash

ABORT="\033[31mAbort:\033[00m"
SUCCESS="\033[32mSuccess:\033[00m"
INFO="\033[33mInfo:\033[00m"

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
    cmd //c 'rmdir '$BACKSLASHED_ROOT_DIR'\build\win32\Debug\app\data' &
    cmd //c 'rmdir '$BACKSLASHED_ROOT_DIR'\build\win32\Debug\app\user' &
    cmd //c 'rmdir '$BACKSLASHED_ROOT_DIR'\build\win32\Release\app\data' &
    cmd //c 'rmdir '$BACKSLASHED_ROOT_DIR'\build\win32\Release\app\user' &
    cmd //c 'rmdir '$BACKSLASHED_ROOT_DIR'\build\win64\Debug\app\data' &
    cmd //c 'rmdir '$BACKSLASHED_ROOT_DIR'\build\win64\Debug\app\user' &
    cmd //c 'rmdir '$BACKSLASHED_ROOT_DIR'\build\win64\Release\app\data' &
    cmd //c 'rmdir '$BACKSLASHED_ROOT_DIR'\build\win64\Release\app\user' &
fi

# Remove folders and contents
rm -rf java_indexer/bin
rm -rf build

# Remove log files
find bin/test/data/log -type f -name 'log*' -delete

echo -e $SUCCESS "clean complete"
