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

CLASSPATH=""
if [ $PLATFORM == "Windows" ]; then
	LIB_DIR="./lib/"
	CLASSPATH+=$LIB_DIR"java-symbol-solver-core.jar;"
	CLASSPATH+=$LIB_DIR"java-symbol-solver-logic.jar;"
	CLASSPATH+=$LIB_DIR"java-symbol-solver-model.jar;"
	CLASSPATH+=$LIB_DIR"javaparser-core.jar"
else
	CLASSPATH+=$ROOT_DIR"/lib/java-symbol-solver-core.jar:"
	CLASSPATH+=$ROOT_DIR"/lib/java-symbol-solver-logic.jar:"
	CLASSPATH+=$ROOT_DIR"/lib/java-symbol-solver-model.jar:"
	CLASSPATH+=$ROOT_DIR"/lib/javaparser-core.jar"
fi

javac -d ./classes -classpath $CLASSPATH src/com/sourcetrail/*.java

mkdir -p bin

cd classes

jar cvf ../bin/java-indexer.jar com/sourcetrail/*.class

cd ..

rm -rf classes
