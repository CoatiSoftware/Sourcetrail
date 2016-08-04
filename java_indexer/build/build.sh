#!/bin/bash

# prerequirements: java/bin in path

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

cd $ROOT_DIR

mkdir -p classes

LIB_DIR="./lib/"
CLASSPATH=""
CLASSPATH+=$LIB_DIR"java-symbol-solver-core-0.2.0-SNAPSHOT.jar;"
CLASSPATH+=$LIB_DIR"java-symbol-solver-logic-0.2.0-SNAPSHOT.jar;"
CLASSPATH+=$LIB_DIR"java-symbol-solver-model-0.2.0-SNAPSHOT.jar;"
CLASSPATH+=$LIB_DIR"javaparser-core-2.4.1-SNAPSHOT.jar;"

javac.exe -d ./classes -classpath $CLASSPATH src/io/coati/*.java

mkdir -p bin

cd classes

jar cvf ../bin/java-indexer.jar io/coati/*.class

cd ..

rm -rf classes
