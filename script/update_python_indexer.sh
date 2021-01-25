#!/bin/bash

# Determine current platform
PLATFORM='unknown'
if [ "$(uname)" == "Darwin" ]; then
	PLATFORM='osx'
elif [ "$(expr substr $(uname -s) 1 5)" == "Linux" ]; then
	PLATFORM='linux'
elif [ "$(expr substr $(uname -s) 1 10)" == "MINGW32_NT" ]; then
	PLATFORM='windows'
elif [ "$(expr substr $(uname -s) 1 10)" == "MINGW64_NT" ]; then
	PLATFORM='windows'
elif [ "$OSTYPE" == "msys" ]; then
	PLATFORM='windows'
fi

INPUT_DIR="submodules/SourcetrailPythonIndexer"
OUTPUT_DIR="bin/app/data/python/indexer"

ABORT="\033[31mAbort:\033[00m"
SUCCESS="\033[32mSuccess:\033[00m"
INFO="\033[33mInfo:\033[00m"

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

echo "Copy python indexer files to $OUTPUT_DIR"
mkdir -p $OUTPUT_DIR
cp $INPUT_DIR/_version.py $OUTPUT_DIR
cp $INPUT_DIR/run.py $OUTPUT_DIR
cp $INPUT_DIR/indexer.py $OUTPUT_DIR
cp $INPUT_DIR/shallow_indexer.py $OUTPUT_DIR
