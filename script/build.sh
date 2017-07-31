#!/bin/bash

# Determine path to script
MY_PATH=`dirname "$0"`

# build target
$MY_PATH/buildonly.sh $@
if [ $? -ne 0 ]
then
	exit 1;
fi

cd $MY_PATH/..

# determine if needs to load project
if [ ! -z "$3" ]
then
	PROJECT_PATH="-p $3"
fi

# run target

if [ "$1" = "release" ] || [ "$1" = "r" ] || [ "$1" = "" ]
then
	if [ "$2" = "test" ]
	then
		#echo "release test"
		cd build/Release/test/ && ./Sourcetrail_test
	elif [ "$2" = "keygen" ]
	then
		#echo "release keygen"
		cd build/Release/license_generator && ./Sourcetrail_license_generator
	else
		#echo "release app"
		cd build/Release/app && ./Sourcetrail $PROJECT_PATH
	fi
elif [ "$1" = "debug" ] || [ "$1" = "d" ]
then
	if [ "$2" = "test" ]
	then
		#echo "debug test"
		cd build/Debug/test && ./Sourcetrail_test
	elif [ "$2" = "keygen" ]
	then
		#echo "debug keygen"
		cd build/Debug/license_generator && ./Sourcetrail_license_generator
	else
		#echo "debug app"
		cd build/Debug/app && ./Sourcetrail $PROJECT_PATH
	fi
else
	echo "no arguments: first argument 'release' or 'debug', second argument 'test' for tests"
fi
