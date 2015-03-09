#!/bin/bash

# Determine path to script
MY_PATH=`dirname "$0"`

cd $MY_PATH/..

# Build and run target

if [ "$1" = "release" ] || [ "$1" = "r" ]
then
	if [ "$2" = "test" ]
	then
		#echo "release test"
		ninja -C build/Release Coati_test && cd bin/test && Release/Coati_test
	else
		#echo "release app"
		ninja -C build/Release Coati && cd bin/app && Release/Coati
	fi
elif [ "$1" = "debug" ] || [ "$1" = "d" ]
then
	if [ "$2" = "test" ]
	then
		#echo "debug test"
		ninja -C build/Debug Coati_test && cd bin/test && Debug/Coati_test
	else
		#echo "debug app"
		ninja -C build/Debug Coati && cd bin/app && Debug/Coati
	fi
else
	echo "no arguments: first argument 'release' or 'debug', second argument 'test' for tests"
fi
