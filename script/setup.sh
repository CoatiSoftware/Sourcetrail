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
	mkdir -p build/win32/Debug/app/platforms
	mkdir -p build/win32/Debug/app/imageformats
	mkdir -p build/win32/Debug/test/platforms
	mkdir -p build/win32/Debug/test/imageformats
	mkdir -p build/win32/Release/app/platforms
	mkdir -p build/win32/Release/app/imageformats
	mkdir -p build/win32/Release/test/platforms
	mkdir -p build/win32/Release/test/imageformats
	mkdir -p build/win64/Debug/app/platforms
	mkdir -p build/win64/Debug/app/imageformats
	mkdir -p build/win64/Debug/test/platforms
	mkdir -p build/win64/Debug/test/imageformats
	mkdir -p build/win64/Release/app/platforms
	mkdir -p build/win64/Release/app/imageformats
	mkdir -p build/win64/Release/test/platforms
	mkdir -p build/win64/Release/test/imageformats
else
	mkdir -p build/Debug/app
	mkdir -p build/Debug/test
	mkdir -p build/Release/app
	mkdir -p build/Release/test
fi

# Copy necessary dynamic libraries to bin folder
if [ $PLATFORM == "Windows" ]; then
	echo -e $INFO "copy dynamic libraries for app"
	cp -u -r setup/dynamic_libraries/win32/app/Debug/* build/win32/Debug/app
	cp -u -r ${QT_WIN32_DIR}/bin/Qt5Cored.dll build/win32/Debug/app
	cp -u -r ${QT_WIN32_DIR}/bin/Qt5Guid.dll build/win32/Debug/app
	cp -u -r ${QT_WIN32_DIR}/bin/Qt5Networkd.dll build/win32/Debug/app
	cp -u -r ${QT_WIN32_DIR}/bin/Qt5Widgetsd.dll build/win32/Debug/app
	cp -u -r ${QT_WIN32_DIR}/plugins/platforms/qwindowsd.dll build/win32/Debug/app/platforms
	cp -u -r ${QT_WIN32_DIR}/plugins/imageformats/qgifd.dll build/win32/Debug/app/imageformats
	cp -u -r ${QT_WIN32_DIR}/plugins/imageformats/qicnsd.dll build/win32/Debug/app/imageformats
	cp -u -r ${QT_WIN32_DIR}/plugins/imageformats/qicod.dll build/win32/Debug/app/imageformats
	cp -u -r ${QT_WIN32_DIR}/plugins/imageformats/qjpegd.dll build/win32/Debug/app/imageformats
	cp -u -r ${QT_WIN32_DIR}/plugins/imageformats/qsvgd.dll build/win32/Debug/app/imageformats
	cp -u -r ${QT_WIN32_DIR}/plugins/imageformats/qtgad.dll build/win32/Debug/app/imageformats
	cp -u -r ${QT_WIN32_DIR}/plugins/imageformats/qtiffd.dll build/win32/Debug/app/imageformats
	cp -u -r ${QT_WIN32_DIR}/plugins/imageformats/qwbmpd.dll build/win32/Debug/app/imageformats
	cp -u -r ${QT_WIN32_DIR}/plugins/imageformats/qwebpd.dll build/win32/Debug/app/imageformats
	
	cp -u -r setup/dynamic_libraries/win32/app/Release/* build/win32/Release/app
	cp -u -r ${QT_WIN32_DIR}/bin/Qt5Core.dll build/win32/Release/app
	cp -u -r ${QT_WIN32_DIR}/bin/Qt5Gui.dll build/win32/Release/app
	cp -u -r ${QT_WIN32_DIR}/bin/Qt5Network.dll build/win32/Release/app
	cp -u -r ${QT_WIN32_DIR}/bin/Qt5Widgets.dll build/win32/Release/app
	cp -u -r ${QT_WIN32_DIR}/plugins/platforms/qwindows.dll build/win32/Release/app/platforms
	cp -u -r ${QT_WIN32_DIR}/plugins/imageformats/qgif.dll build/win32/Release/app/imageformats
	cp -u -r ${QT_WIN32_DIR}/plugins/imageformats/qicns.dll build/win32/Release/app/imageformats
	cp -u -r ${QT_WIN32_DIR}/plugins/imageformats/qico.dll build/win32/Release/app/imageformats
	cp -u -r ${QT_WIN32_DIR}/plugins/imageformats/qjpeg.dll build/win32/Release/app/imageformats
	cp -u -r ${QT_WIN32_DIR}/plugins/imageformats/qsvg.dll build/win32/Release/app/imageformats
	cp -u -r ${QT_WIN32_DIR}/plugins/imageformats/qtga.dll build/win32/Release/app/imageformats
	cp -u -r ${QT_WIN32_DIR}/plugins/imageformats/qtiff.dll build/win32/Release/app/imageformats
	cp -u -r ${QT_WIN32_DIR}/plugins/imageformats/qwbmp.dll build/win32/Release/app/imageformats
	cp -u -r ${QT_WIN32_DIR}/plugins/imageformats/qwebp.dll build/win32/Release/app/imageformats
	
	cp -u -r setup/dynamic_libraries/win64/app/Debug/* build/win64/Debug/app
	cp -u -r ${QT_WIN64_DIR}/bin/Qt5Cored.dll build/win64/Debug/app
	cp -u -r ${QT_WIN64_DIR}/bin/Qt5Guid.dll build/win64/Debug/app
	cp -u -r ${QT_WIN64_DIR}/bin/Qt5Networkd.dll build/win64/Debug/app
	cp -u -r ${QT_WIN64_DIR}/bin/Qt5Widgetsd.dll build/win64/Debug/app
	cp -u -r ${QT_WIN64_DIR}/plugins/platforms/qwindowsd.dll build/win64/Debug/app/platforms
	cp -u -r ${QT_WIN64_DIR}/plugins/imageformats/qgifd.dll build/win64/Debug/app/imageformats
	cp -u -r ${QT_WIN64_DIR}/plugins/imageformats/qicnsd.dll build/win64/Debug/app/imageformats
	cp -u -r ${QT_WIN64_DIR}/plugins/imageformats/qicod.dll build/win64/Debug/app/imageformats
	cp -u -r ${QT_WIN64_DIR}/plugins/imageformats/qjpegd.dll build/win64/Debug/app/imageformats
	cp -u -r ${QT_WIN64_DIR}/plugins/imageformats/qsvgd.dll build/win64/Debug/app/imageformats
	cp -u -r ${QT_WIN64_DIR}/plugins/imageformats/qtgad.dll build/win64/Debug/app/imageformats
	cp -u -r ${QT_WIN64_DIR}/plugins/imageformats/qtiffd.dll build/win64/Debug/app/imageformats
	cp -u -r ${QT_WIN64_DIR}/plugins/imageformats/qwbmpd.dll build/win64/Debug/app/imageformats
	cp -u -r ${QT_WIN64_DIR}/plugins/imageformats/qwebpd.dll build/win64/Debug/app/imageformats
	
	cp -u -r setup/dynamic_libraries/win64/app/Release/* build/win64/Release/app
	cp -u -r ${QT_WIN64_DIR}/bin/Qt5Core.dll build/win64/Release/app
	cp -u -r ${QT_WIN64_DIR}/bin/Qt5Gui.dll build/win64/Release/app
	cp -u -r ${QT_WIN64_DIR}/bin/Qt5Network.dll build/win64/Release/app
	cp -u -r ${QT_WIN64_DIR}/bin/Qt5Widgets.dll build/win64/Release/app
	cp -u -r ${QT_WIN64_DIR}/plugins/platforms/qwindows.dll build/win64/Release/app/platforms
	cp -u -r ${QT_WIN64_DIR}/plugins/imageformats/qgif.dll build/win64/Release/app/imageformats
	cp -u -r ${QT_WIN64_DIR}/plugins/imageformats/qicns.dll build/win64/Release/app/imageformats
	cp -u -r ${QT_WIN64_DIR}/plugins/imageformats/qico.dll build/win64/Release/app/imageformats
	cp -u -r ${QT_WIN64_DIR}/plugins/imageformats/qjpeg.dll build/win64/Release/app/imageformats
	cp -u -r ${QT_WIN64_DIR}/plugins/imageformats/qsvg.dll build/win64/Release/app/imageformats
	cp -u -r ${QT_WIN64_DIR}/plugins/imageformats/qtga.dll build/win64/Release/app/imageformats
	cp -u -r ${QT_WIN64_DIR}/plugins/imageformats/qtiff.dll build/win64/Release/app/imageformats
	cp -u -r ${QT_WIN64_DIR}/plugins/imageformats/qwbmp.dll build/win64/Release/app/imageformats
	cp -u -r ${QT_WIN64_DIR}/plugins/imageformats/qwebp.dll build/win64/Release/app/imageformats
	
	
	echo -e $INFO "copy dynamic libraries for tests"
	cp -u -r ${QT_WIN32_DIR}/bin/Qt5Cored.dll build/win32/Debug/test
	cp -u -r ${QT_WIN32_DIR}/bin/Qt5Guid.dll build/win32/Debug/test
	cp -u -r ${QT_WIN32_DIR}/bin/Qt5Networkd.dll build/win32/Debug/test
	cp -u -r ${QT_WIN32_DIR}/bin/Qt5Widgetsd.dll build/win32/Debug/test
	cp -u -r ${QT_WIN32_DIR}/plugins/platforms/qwindowsd.dll build/win32/Debug/test/platforms
	cp -u -r ${QT_WIN32_DIR}/plugins/imageformats/qgifd.dll build/win32/Debug/test/imageformats
	cp -u -r ${QT_WIN32_DIR}/plugins/imageformats/qicnsd.dll build/win32/Debug/test/imageformats
	cp -u -r ${QT_WIN32_DIR}/plugins/imageformats/qicod.dll build/win32/Debug/test/imageformats
	cp -u -r ${QT_WIN32_DIR}/plugins/imageformats/qjpegd.dll build/win32/Debug/test/imageformats
	cp -u -r ${QT_WIN32_DIR}/plugins/imageformats/qsvgd.dll build/win32/Debug/test/imageformats
	cp -u -r ${QT_WIN32_DIR}/plugins/imageformats/qtgad.dll build/win32/Debug/test/imageformats
	cp -u -r ${QT_WIN32_DIR}/plugins/imageformats/qtiffd.dll build/win32/Debug/test/imageformats
	cp -u -r ${QT_WIN32_DIR}/plugins/imageformats/qwbmpd.dll build/win32/Debug/test/imageformats
	cp -u -r ${QT_WIN32_DIR}/plugins/imageformats/qwebpd.dll build/win32/Debug/test/imageformats
	
	cp -u -r ${QT_WIN32_DIR}/bin/Qt5Core.dll build/win32/Release/test
	cp -u -r ${QT_WIN32_DIR}/bin/Qt5Gui.dll build/win32/Release/test
	cp -u -r ${QT_WIN32_DIR}/bin/Qt5Network.dll build/win32/Release/test
	cp -u -r ${QT_WIN32_DIR}/bin/Qt5Widgets.dll build/win32/Release/test
	cp -u -r ${QT_WIN32_DIR}/plugins/platforms/qwindows.dll build/win32/Release/test/platforms
	cp -u -r ${QT_WIN32_DIR}/plugins/imageformats/qgif.dll build/win32/Release/test/imageformats
	cp -u -r ${QT_WIN32_DIR}/plugins/imageformats/qicns.dll build/win32/Release/test/imageformats
	cp -u -r ${QT_WIN32_DIR}/plugins/imageformats/qico.dll build/win32/Release/test/imageformats
	cp -u -r ${QT_WIN32_DIR}/plugins/imageformats/qjpeg.dll build/win32/Release/test/imageformats
	cp -u -r ${QT_WIN32_DIR}/plugins/imageformats/qsvg.dll build/win32/Release/test/imageformats
	cp -u -r ${QT_WIN32_DIR}/plugins/imageformats/qtga.dll build/win32/Release/test/imageformats
	cp -u -r ${QT_WIN32_DIR}/plugins/imageformats/qtiff.dll build/win32/Release/test/imageformats
	cp -u -r ${QT_WIN32_DIR}/plugins/imageformats/qwbmp.dll build/win32/Release/test/imageformats
	cp -u -r ${QT_WIN32_DIR}/plugins/imageformats/qwebp.dll build/win32/Release/test/imageformats
	
	cp -u -r ${QT_WIN64_DIR}/bin/Qt5Cored.dll build/win64/Debug/test
	cp -u -r ${QT_WIN64_DIR}/bin/Qt5Guid.dll build/win64/Debug/test
	cp -u -r ${QT_WIN64_DIR}/bin/Qt5Networkd.dll build/win64/Debug/test
	cp -u -r ${QT_WIN64_DIR}/bin/Qt5Widgetsd.dll build/win64/Debug/test
	cp -u -r ${QT_WIN64_DIR}/plugins/platforms/qwindowsd.dll build/win64/Debug/test/platforms
	cp -u -r ${QT_WIN64_DIR}/plugins/imageformats/qgifd.dll build/win64/Debug/test/imageformats
	cp -u -r ${QT_WIN64_DIR}/plugins/imageformats/qicnsd.dll build/win64/Debug/test/imageformats
	cp -u -r ${QT_WIN64_DIR}/plugins/imageformats/qicod.dll build/win64/Debug/test/imageformats
	cp -u -r ${QT_WIN64_DIR}/plugins/imageformats/qjpegd.dll build/win64/Debug/test/imageformats
	cp -u -r ${QT_WIN64_DIR}/plugins/imageformats/qsvgd.dll build/win64/Debug/test/imageformats
	cp -u -r ${QT_WIN64_DIR}/plugins/imageformats/qtgad.dll build/win64/Debug/test/imageformats
	cp -u -r ${QT_WIN64_DIR}/plugins/imageformats/qtiffd.dll build/win64/Debug/test/imageformats
	cp -u -r ${QT_WIN64_DIR}/plugins/imageformats/qwbmpd.dll build/win64/Debug/test/imageformats
	cp -u -r ${QT_WIN64_DIR}/plugins/imageformats/qwebpd.dll build/win64/Debug/test/imageformats
	
	cp -u -r ${QT_WIN64_DIR}/bin/Qt5Core.dll build/win64/Release/test
	cp -u -r ${QT_WIN64_DIR}/bin/Qt5Gui.dll build/win64/Release/test
	cp -u -r ${QT_WIN64_DIR}/bin/Qt5Network.dll build/win64/Release/test
	cp -u -r ${QT_WIN64_DIR}/bin/Qt5Widgets.dll build/win64/Release/test
	cp -u -r ${QT_WIN64_DIR}/plugins/platforms/qwindows.dll build/win64/Release/test/platforms
	cp -u -r ${QT_WIN64_DIR}/plugins/imageformats/qgif.dll build/win64/Release/test/imageformats
	cp -u -r ${QT_WIN64_DIR}/plugins/imageformats/qicns.dll build/win64/Release/test/imageformats
	cp -u -r ${QT_WIN64_DIR}/plugins/imageformats/qico.dll build/win64/Release/test/imageformats
	cp -u -r ${QT_WIN64_DIR}/plugins/imageformats/qjpeg.dll build/win64/Release/test/imageformats
	cp -u -r ${QT_WIN64_DIR}/plugins/imageformats/qsvg.dll build/win64/Release/test/imageformats
	cp -u -r ${QT_WIN64_DIR}/plugins/imageformats/qtga.dll build/win64/Release/test/imageformats
	cp -u -r ${QT_WIN64_DIR}/plugins/imageformats/qtiff.dll build/win64/Release/test/imageformats
	cp -u -r ${QT_WIN64_DIR}/plugins/imageformats/qwbmp.dll build/win64/Release/test/imageformats
	cp -u -r ${QT_WIN64_DIR}/plugins/imageformats/qwebp.dll build/win64/Release/test/imageformats

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
