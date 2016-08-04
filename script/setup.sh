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
cd $ROOT_DIR


# git settings
echo -e $INFO "install git settings"

git config commit.template setup/git/git_commit_template.txt
git config color.ui true
cp setup/git/git_pre_commit_hook.sh .git/hooks/pre-commit
cp setup/git/git_pre_push_hook.sh .git/hooks/pre-push


# Copy necessary jars for java indexer
echo -e $INFO "copy jars for java indexer"

mkdir -p java_indexer/lib

if [ $PLATFORM == "Windows" ]; then
	cp -u -r setup/jars/windows/*.jar java_indexer/lib
elif [ $PLATFORM == "Linux" ]; then
	cp -u -r setup/jars/linux/*.jar java_indexer/lib
	# what about 32/64 bit?
elif [ $PLATFORM == "MacOS" ]; then
	cp -u -r setup/jars/macos/*.jar java_indexer/lib
fi


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

	BACKSLASHED_ROOT_DIR="${ROOT_DIR//\//\\}"

	echo -e $INFO "create symbolic links for data"
	cmd //c 'mklink /d /j '$BACKSLASHED_ROOT_DIR'\bin\app\Debug\data '$BACKSLASHED_ROOT_DIR'\bin\app\data' &
	cmd //c 'mklink /d /j '$BACKSLASHED_ROOT_DIR'\bin\app\Debug\user '$BACKSLASHED_ROOT_DIR'\bin\app\user' &
	cmd //c 'mklink /d /j '$BACKSLASHED_ROOT_DIR'\bin\app\Release\data '$BACKSLASHED_ROOT_DIR'\bin\app\data' &
	cmd //c 'mklink /d /j '$BACKSLASHED_ROOT_DIR'\bin\app\Release\user '$BACKSLASHED_ROOT_DIR'\bin\app\user' &
elif [ $PLATFORM == "Linux" ]; then
	echo -e $INFO "create symbolic links for data"
	cd $ROOT_DIR/bin/app/Release
	ln -s ../data
	cd $ROOT_DIR/bin/app/Debug
	ln -s ../data
	cd $ROOT_DIR
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
