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

# Enter masterproject directory
MY_PATH=`dirname "$0"`
cd $MY_PATH/..

# git settings
echo -e $INFO "install git settings"

git config commit.template .git_commit_template.txt
git config color.ui true
cp .git_pre_commit_hook.sh .git/hooks/pre-commit


# Create Debug and Release folders
echo -e $INFO "create build folders"

mkdir -p build
mkdir -p bin/app/Debug
mkdir -p bin/app/Release
mkdir -p bin/lib/Debug
mkdir -p bin/lib/Release
mkdir -p bin/test/Debug
mkdir -p bin/test/Release

# Copy necessary dynamic libraries to bin folder
if [ $PLATFORM == "Windows" ]; then
	echo -e $INFO "copy dynamic libraries"
	cp -u -r setup/dynamic_libraries/windows/app/Debug/* bin/app/Debug
	cp -u -r setup/dynamic_libraries/windows/app/Release/* bin/app/Release

	echo -e $INFO "copy test_main file"
	cp -u setup/cxx_test/windows/test_main.cpp build

	echo -e $INFO "creating program icon"
	sh script/create_windows_icon.sh

	cmd //c 'mklink /d /j '.$MY_PATH.'\..\bin\app\Debug\data '.$MY_PATH.'\..\bin\app\data' &
	cmd //c 'mklink /d /j '.$MY_PATH.'\..\bin\app\Debug\user '.$MY_PATH.'\..\bin\app\user' &
	cmd //c 'mklink /d /j '.$MY_PATH.'\..\bin\app\Release\data '.$MY_PATH.'\..\bin\app\data' &
	cmd //c 'mklink /d /j '.$MY_PATH.'\..\bin\app\Release\user '.$MY_PATH.'\..\bin\app\user' &

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
