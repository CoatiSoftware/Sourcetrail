#!/bin/bash

set -e
# Determine path to script
MY_PATH=`dirname "$0"`

cd $MY_PATH/..

# run setup script if needed
if [ ! -d "build/Release" ]
then
	$MY_PATH/setup.sh
	if [ $? -ne 0 ]
	then
		exit 1;
	fi
else
	echo "build exists"
fi

# Build target

if [ "$1" = "release" ] || [ "$1" = "r" ] || [ "$1" = "" ]
then
	if [ "$2" = "test" ]
	then
		#echo "release test"
		cmake --build build/Release --target Sourcetrail_test
	elif [ "$2" = "keygen" ]
	then
		#echo "release keygen"
		cmake --build build/Release --target Sourcetrail_license_generator
	else
		#echo "release app"
		cmake --build build/Release --target Sourcetrail
	fi
elif [ "$1" = "debug" ] || [ "$1" = "d" ]
then
	if [ "$2" = "test" ]
	then
		#echo "debug test"
		cmake --build build/Debug --target Sourcetrail_test
	elif [ "$2" = "keygen" ]
	then
		#echo "debug keygen"
		cmake --build build/Debug --target Sourcetrail_license_generator
	else
		#echo "debug app"
		cmake --build build/Debug --target Sourcetrail
	fi
elif [ "$1" = "package" ] || [ "$1" = "p" ]
then
		cmake --build build/Release --target package
		mv build/Release/Sourcetrail*.tar.gz . #&& cp build/Release/Sourcetrail*.deb distr
		echo "Packages copied into the root folder"
elif [ "$1" = "all" ] || [ "$1" = "a" ]
then
	cmake --build build/Release --target all
else
	echo "no arguments: first argument 'release' or 'debug', second argument 'test' for tests"
fi
