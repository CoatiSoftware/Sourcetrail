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
	PLATFORM='windows'
elif [ "$(expr substr $(uname -s) 1 10)" == "MINGW64_NT" ]; then
	PLATFORM='windows'
elif [ "$OSTYPE" == "msys" ]; then
	PLATFORM='windows'
fi

if [ $PLATFORM == "windows" ]; then
	SCRIPT=`realpath $0`
	if [ "$SCRIPT" == "" ]; then

		ORIGINAL_PATH_TO_SCRIPT="${0}"
		CLEANED_PATH_TO_SCRIPT="${ORIGINAL_PATH_TO_SCRIPT//\\//}"
		SCRIPT_DIR=${CLEANED_PATH_TO_SCRIPT%/*}
	else
		ORIGINAL_PATH_TO_SCRIPT=`dirname $SCRIPT`
		SCRIPT_DIR="${ORIGINAL_PATH_TO_SCRIPT//\\//}"
	fi
else
	SCRIPT_DIR="$( cd "$( dirname "$0" )" && pwd )"
fi

echo "This script is running in: $SCRIPT_DIR"

# Enter main directory
cd $SCRIPT_DIR/
cd ..

cd java_indexer

# exit when any command fails
set -e 

echo -e $INFO "cleaning and building java_indexer.jar"
mvn clean package

cd ../

echo -e $INFO "copy jars"
mkdir -p bin/app/data/java/lib/
rm -rf bin/app/data/java/lib/*
cp -r java_indexer/target/java-indexer-1.0.jar bin/app/data/java/lib/java-indexer.jar
cp -r java_indexer/lib/*.jar bin/app/data/java/lib
