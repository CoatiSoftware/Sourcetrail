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

# Enter main directory
cd $ROOT_DIR/

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
