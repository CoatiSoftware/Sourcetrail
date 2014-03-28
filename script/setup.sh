#!/bin/sh

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

# Enter masterproject directory
MY_PATH=`dirname "$0"`
cd $MY_PATH/..

# git settings
echo $INFO "install git settings"

git config commit.template .git_commit_template.txt
git config color.ui true
cp .git_pre_commit_hook.sh .git/hooks/pre-commit


# Create Debug and Release folders
echo $INFO "create build folders"

mkdir -p build
mkdir -p lib
mkdir -p bin/Debug
mkdir -p bin/Release

# Copy necessary dynamic libraries to bin folder
if [ $PLATFORM == "Windows" ]; then
	echo $INFO "copy dynamic libraries"
	cp -u -r setup/dynamic_libraries/windows/Debug/* bin/Debug
	cp -u -r setup/dynamic_libraries/windows/Release/* bin/Release
fi

# Setup both Debug and Release configuration
if [ $PLATFORM == "Linux" ] || [ $PLATFORM == "MacOS" ]; then
	mkdir -p build/Debug
	mkdir -p build/Release

	echo $INFO "run cmake with Debug configuration"
	cd build/Debug && cmake -DCMAKE_BUILD_TYPE="Debug" ../..

	echo $INFO "run cmake with Release configuration"
	cd ../Release && cmake -DCMAKE_BUILD_TYPE="Release" ../..
fi

echo $SUCCESS "setup complete"
