#!/bin/bash

set -e

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
cd $ROOT_DIR

# git settings
echo -e $INFO "install git settings"

git config commit.template setup/git/git_commit_template.txt
git config color.ui true
if [ -d ".git/hooks" ]; then
	cp setup/git/git_pre_commit_hook.sh .git/hooks/pre-commit
	cp setup/git/git_pre_push_hook.sh .git/hooks/pre-push
fi

# Create Debug and Release folders
echo -e $INFO "create build folders"
if [ $PLATFORM == "Windows" ]; then
else
	mkdir -p build/Debug/app
	mkdir -p build/Debug/test
	mkdir -p build/Release/app
	mkdir -p build/Release/test
fi

if [ $PLATFORM == "Windows" ]; then
else
	MY_PATH=`dirname "$0"`
	$MY_PATH/download_python_indexer.sh
fi


# Setup both Debug and Release configuration
if [ $PLATFORM == "Linux" ] || [ $PLATFORM == "MacOS" ]; then
	mkdir -p build/Debug
	mkdir -p build/Release

	echo -e $INFO "run cmake with Debug configuration"
	cd build/Debug && cmake -G Ninja -DCMAKE_BUILD_TYPE="Debug" ../..

	echo -e $INFO "run cmake with Release configuration"
	cd ../Release && cmake -G Ninja -DCMAKE_BUILD_TYPE="Release" ../..
fi

echo -e $SUCCESS "setup complete"
