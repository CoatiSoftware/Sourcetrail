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
if [ -d ".git/hooks" ]
then
cp setup/git/git_pre_commit_hook.sh .git/hooks/pre-commit
cp setup/git/git_pre_push_hook.sh .git/hooks/pre-push
fi

# Create Debug and Release folders
echo -e $INFO "create build folders"
if [ $PLATFORM == "Windows" ]; then

	copy_dynamic_libraries() # Parameters: bit {32, 64}, mode {Debug, Release}, target {app, test}
	{
		mkdir -p build/win$1/$2/$3/platforms
		mkdir -p build/win$1/$2/$3/imageformats
		
		local QT_DIR=${QT_WIN32_DIR}
		
		if [[ "$1" == "64" ]]
		then
			QT_DIR=${QT_WIN64_DIR}
		fi
		
		local SUFFIX=""
		if [[ "$2" == "Debug" ]]
		then
			SUFFIX="d"
		fi
		
		echo -e $INFO "copy dynamic libraries for Win$1 $2 $3"
		
		if [[ "$3" == "app" ]]
		then
			cp -u -r setup/dynamic_libraries/win$1/$3/$2/* build/win$1/$2/$3
		fi
		
		cp -u -r ${QT_DIR}/bin/Qt5Core$SUFFIX.dll build/win$1/$2/$3
		cp -u -r ${QT_DIR}/bin/Qt5Gui$SUFFIX.dll build/win$1/$2/$3
		cp -u -r ${QT_DIR}/bin/Qt5Network$SUFFIX.dll build/win$1/$2/$3
		cp -u -r ${QT_DIR}/bin/Qt5Widgets$SUFFIX.dll build/win$1/$2/$3
		cp -u -r ${QT_DIR}/bin/Qt5WinExtras$SUFFIX.dll build/win$1/$2/$3
		cp -u -r ${QT_DIR}/plugins/platforms/qwindows$SUFFIX.dll build/win$1/$2/$3/platforms
		cp -u -r ${QT_DIR}/plugins/imageformats/qgif$SUFFIX.dll build/win$1/$2/$3/imageformats
		cp -u -r ${QT_DIR}/plugins/imageformats/qicns$SUFFIX.dll build/win$1/$2/$3/imageformats
		cp -u -r ${QT_DIR}/plugins/imageformats/qico$SUFFIX.dll build/win$1/$2/$3/imageformats
		cp -u -r ${QT_DIR}/plugins/imageformats/qjpeg$SUFFIX.dll build/win$1/$2/$3/imageformats
		cp -u -r ${QT_DIR}/plugins/imageformats/qsvg$SUFFIX.dll build/win$1/$2/$3/imageformats
		cp -u -r ${QT_DIR}/plugins/imageformats/qtga$SUFFIX.dll build/win$1/$2/$3/imageformats
		cp -u -r ${QT_DIR}/plugins/imageformats/qtiff$SUFFIX.dll build/win$1/$2/$3/imageformats
		cp -u -r ${QT_DIR}/plugins/imageformats/qwbmp$SUFFIX.dll build/win$1/$2/$3/imageformats
		cp -u -r ${QT_DIR}/plugins/imageformats/qwebp$SUFFIX.dll build/win$1/$2/$3/imageformats
	}
	
	copy_dynamic_libraries "32" "Debug" "app"
	copy_dynamic_libraries "32" "Debug" "test"
	copy_dynamic_libraries "32" "Release" "app"
	copy_dynamic_libraries "32" "Release" "test"
	copy_dynamic_libraries "64" "Debug" "app"
	copy_dynamic_libraries "64" "Debug" "test"
	copy_dynamic_libraries "64" "Release" "app"
	copy_dynamic_libraries "64" "Release" "test"
else
	mkdir -p build/Debug/app
	mkdir -p build/Debug/test
	mkdir -p build/Release/app
	mkdir -p build/Release/test
fi


if [ $PLATFORM == "Windows" ]; then
	echo -e $INFO "copy test_main file"
	cp -u setup/cxx_test/windows/test_main.cpp build/win32
	cp -u setup/cxx_test/windows/test_main.cpp build/win64

	echo -e $INFO "creating program icon"
	sh script/create_windows_icon.sh
fi


echo -e $INFO "create symbolic links for data"
if [ $PLATFORM == "Windows" ]; then
	BACKSLASHED_ROOT_DIR="${ROOT_DIR//\//\\}"
	cmd //c 'mklink /d /j '$BACKSLASHED_ROOT_DIR'\build\win32\Debug\app\data '$BACKSLASHED_ROOT_DIR'\bin\app\data' &
	cmd //c 'mklink /d /j '$BACKSLASHED_ROOT_DIR'\build\win32\Debug\app\user '$BACKSLASHED_ROOT_DIR'\bin\app\user' &
	cmd //c 'mklink /d /j '$BACKSLASHED_ROOT_DIR'\build\win32\Release\app\data '$BACKSLASHED_ROOT_DIR'\bin\app\data' &
	cmd //c 'mklink /d /j '$BACKSLASHED_ROOT_DIR'\build\win32\Release\app\user '$BACKSLASHED_ROOT_DIR'\bin\app\user' &
	
	cmd //c 'mklink /d /j '$BACKSLASHED_ROOT_DIR'\build\win64\Debug\app\data '$BACKSLASHED_ROOT_DIR'\bin\app\data' &
	cmd //c 'mklink /d /j '$BACKSLASHED_ROOT_DIR'\build\win64\Debug\app\user '$BACKSLASHED_ROOT_DIR'\bin\app\user' &
	cmd //c 'mklink /d /j '$BACKSLASHED_ROOT_DIR'\build\win64\Release\app\data '$BACKSLASHED_ROOT_DIR'\bin\app\data' &
	cmd //c 'mklink /d /j '$BACKSLASHED_ROOT_DIR'\build\win64\Release\app\user '$BACKSLASHED_ROOT_DIR'\bin\app\user' &
fi

# Setup both Debug and Release configuration
if [ $PLATFORM == "Linux" ] || [ $PLATFORM == "MacOS" ]; then
	mkdir -p build/Debug
	mkdir -p build/Release

	echo -e $INFO "run cmake with Debug configuration"
	cd build/Debug && cmake -G Ninja -DCMAKE_BUILD_TYPE="Debug" ../..

	echo -e $INFO "run cmake with Release configuration"
	cd ../Release && cmake -G Ninja -DCMAKE_BUILD_TYPE="Release" ../..
else
	echo -e $INFO "run cmake with 32 bit configuration"
	
	cd build/win32
	cmake -G "Visual Studio 14 2015" ../..
	
	cd ../win64
	cmake -G "Visual Studio 14 2015 Win64" ../..
fi

echo -e $SUCCESS "setup complete"
