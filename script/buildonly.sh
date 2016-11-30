#!/bin/bash

set -e
# Determine path to script
MY_PATH=`dirname "$0"`

cd $MY_PATH/..

# Build target

if [ "$1" = "release" ] || [ "$1" = "r" ] || [ "$1" = "" ]
then
	if [ "$2" = "test" ]
	then
		#echo "release test"
		cmake --build build/Release --target Coati_test
	elif [ "$2" = "keygen" ]
	then
		#echo "release keygen"
		cmake --build build/Release --target Coati_license_generator
	else
		#echo "release app"
		cmake --build build/Release --target Coati
	fi
elif [ "$1" = "debug" ] || [ "$1" = "d" ]
then
	if [ "$2" = "test" ]
	then
		#echo "debug test"
		cmake --build build/Debug --target Coati_test
	elif [ "$2" = "keygen" ]
	then
		#echo "debug keygen"
		cmake --build build/Debug --target Coati_license_generator
	else
		#echo "debug app"
		cmake --build build/Debug --target Coati
	fi
elif [ "$1" = "package" ] || [ "$1" = "p" ]
then
		cmake --build build/Release --target package
		mkdir -p distr && mv build/Release/Coati*.tar.gz distr #&& cp build/Release/Coati*.deb distr
		echo "Packages copied into the distr folder"
else
	echo "no arguments: first argument 'release' or 'debug', second argument 'test' for tests"
fi
