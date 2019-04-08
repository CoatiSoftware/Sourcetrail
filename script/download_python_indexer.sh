#!/bin/bash

SOURCETRAIL_PYTHON_INDEXER_VERSION="v0_db23_p3"

# Determine current platform
PLATFORM='unknown'
if [ "$(uname)" == "Darwin" ]; then
	PLATFORM='osx'
elif [ "$(expr substr $(uname -s) 1 5)" == "Linux" ]; then
	PLATFORM='linux'
elif [ "$(expr substr $(uname -s) 1 10)" == "MINGW32_NT" ]; then
	PLATFORM='windows'
fi

PACKAGE_NAME="SourcetrailPythonIndexer_${SOURCETRAIL_PYTHON_INDEXER_VERSION}-${PLATFORM}"
PACKAGE_FILE_NAME="${PACKAGE_NAME}.zip"
PACKAGE_URL="https://github.com/CoatiSoftware/SourcetrailPythonIndexer/releases/download/${SOURCETRAIL_PYTHON_INDEXER_VERSION}/${PACKAGE_FILE_NAME}"
TEMP_PATH="build/temp"
TARGET_PATH="bin/app/data/python"


ABORT="\033[31mAbort:\033[00m"
SUCCESS="\033[32mSuccess:\033[00m"
INFO="\033[33mInfo:\033[00m"

if [ $PLATFORM == "windows" ]; then
	ORIGINAL_PATH_TO_SCRIPT="${0}"
	CLEANED_PATH_TO_SCRIPT="${ORIGINAL_PATH_TO_SCRIPT//\\//}"
	ROOT_DIR=`dirname "$CLEANED_PATH_TO_SCRIPT"`
else
	ROOT_DIR="$( cd "$( dirname "$0" )" && pwd )"
fi

ROOT_DIR=$ROOT_DIR/..

# Enter main directory
cd $ROOT_DIR/

mkdir -p $TEMP_PATH

echo -e $INFO "starting to download $PACKAGE_FILE_NAME"

if [ $PLATFORM == "linux" ]; then
	wget -O $TEMP_PATH/$PACKAGE_FILE_NAME $PACKAGE_URL
elif [ $PLATFORM == "osx" ]; then
	wget -O $TEMP_PATH/$PACKAGE_FILE_NAME $PACKAGE_URL
elif [ $PLATFORM == "windows" ]; then
	certutil.exe -urlcache -split -f $PACKAGE_URL $TEMP_PATH/$PACKAGE_FILE_NAME
fi

echo -e $INFO "finished downloading $PACKAGE_FILE_NAME"


if [ $PLATFORM == "linux" ]; then
	unzip -d $TEMP_PATH $TEMP_PATH/$PACKAGE_FILE_NAME
elif [ $PLATFORM == "osx" ]; then
	unzip -d $TEMP_PATH $TEMP_PATH/$PACKAGE_FILE_NAME
elif [ $PLATFORM == "windows" ]; then
	winrar x $TEMP_PATH/$PACKAGE_FILE_NAME $TEMP_PATH
fi


echo -e $INFO "clearing $TARGET_PATH"
rm -rf $TARGET_PATH
mkdir -p $TARGET_PATH
cp -r $TEMP_PATH/$PACKAGE_NAME/* $TARGET_PATH


echo -e $INFO "clearing temporary data at $TEMP_PATH"
rm -rf $TEMP_PATH
