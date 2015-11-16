#!/bin/bash

ABORT="\033[31mAbort:\033[00m"
SUCCESS="\033[32mSuccess:\033[00m"
INFO="\033[33mInfo:\033[00m"

read -r -p "Is your current release build up to date? [y/N] " response
case $response in
	[yY][eE][sS]|[yY]) 
		;;
	*)
		echo -e $ABORT  "Aborting the creation of a release package."
		exit 1
	;;
esac

ORIGINAL_PATH_TO_SCRIPT="${0}"
CLEANED_PATH_TO_SCRIPT="${ORIGINAL_PATH_TO_SCRIPT//\\//}"
BASE_DIR=`dirname "$CLEANED_PATH_TO_SCRIPT"`

cd $BASE_DIR/..

VERSION_STRING=$(git describe)
VERSION_STRING="${VERSION_STRING//-/_}"
VERSION_STRING="${VERSION_STRING//./_}"
VERSION_STRING="${VERSION_STRING%_*}"

PACKAGE_NAME=Coati_${VERSION_STRING}_windows

echo -e $INFO Creating Package with Name: $PACKAGE_NAME

PACKAGE_DIR=release/$PACKAGE_NAME


echo -e $INFO Creating Folders
rm -rf $PACKAGE_DIR
mkdir -p $PACKAGE_DIR

mkdir -p $PACKAGE_DIR/data/color_schemes/
cp -u -r bin/app/data/color_schemes/* $PACKAGE_DIR/data/color_schemes/
mkdir -p $PACKAGE_DIR/data/fonts/
cp -u -r bin/app/data/fonts/* $PACKAGE_DIR/data/fonts/
mkdir -p $PACKAGE_DIR/data/gui/
cp -u -r bin/app/data/gui/* $PACKAGE_DIR/data/gui/
mkdir -p $PACKAGE_DIR/data/log/
mkdir -p $PACKAGE_DIR/data/projects/tutorial/src/
cp -u -r bin/app/data/projects/tutorial/src/* $PACKAGE_DIR/data/projects/tutorial/src/
cp -u -r bin/app/data/projects/tutorial/tutorial.xml $PACKAGE_DIR/data/projects/tutorial/
mkdir -p $PACKAGE_DIR/data/projects/tictactoe/src/
cp -u -r bin/app/data/projects/tictactoe/src/* $PACKAGE_DIR/data/projects/tictactoe/src/
cp -u -r bin/app/data/projects/tictactoe/tictactoe.xml $PACKAGE_DIR/data/projects/tictactoe/

cp -u -r bin/app/data/ApplicationSettings_for_package.xml $PACKAGE_DIR/data/ApplicationSettings.xml
cp -u -r bin/app/data/window_settings.ini $PACKAGE_DIR/data/

mkdir -p $PACKAGE_DIR/plugins/visual_studio/
cp -u -r ide_plugins/vs/coati_plugin_vs_2012.vsix $PACKAGE_DIR/plugins/visual_studio/
cp -u -r ide_plugins/vs/coati_plugin_vs_2013.vsix $PACKAGE_DIR/plugins/visual_studio/
cp -u -r ide_plugins/vs/coati_plugin_vs_2015.vsix $PACKAGE_DIR/plugins/visual_studio/
mkdir -p $PACKAGE_DIR/plugins/sublime_text/
cp -u -r ide_plugins/sublime_text/* $PACKAGE_DIR/plugins/sublime_text/

cp -u -r web/coati_manual.pdf $PACKAGE_DIR/
cp -u -r bin/app/Release/Coati.exe $PACKAGE_DIR/
cp -u -r setup/dynamic_libraries/windows/app/Release/* $PACKAGE_DIR/


echo -e $INFO Obfuscation Executable
#upx --brute $PACKAGE_DIR/Coati.exe

echo -e $INFO Packaging Coati
cd ./release/
winrar a -afzip $PACKAGE_NAME.zip $PACKAGE_NAME
cd ../


echo -e $INFO Cleaning up
rm -rf $PACKAGE_DIR

echo -e $SUCCESS Packaging Complete!