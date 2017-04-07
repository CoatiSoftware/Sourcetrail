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

# run target

if [ "$1" = "release" ] || [ "$1" = "r" ]
then
	if [ "$2" = "test" ]
	then
		#echo "release test"
		cd bin/test && ../../build/Release/test/Sourcetrail_test
	elif [ "$2" = "keygen" ]
	then
		#echo "release keygen"
		cd bin/license_generator && ../../build/Release/license_generator/Sourcetrail_license_generator
	else
		#echo "release app"
		cd bin/app && ../../build/Release/app/Sourcetrail
	fi
elif [ "$1" = "debug" ] || [ "$1" = "d" ]
then
	if [ "$2" = "test" ]
	then
		#echo "debug test"
		cd bin/test && ../../build/Debug/test/Sourcetrail_test
	elif [ "$2" = "keygen" ]
	then
		#echo "debug keygen"
		cd bin/license_generator && ../../build/Debug/license_generator/Sourcetrail_license_generator
	else
		#echo "debug app"
		cd bin/app && ../../build/Debug/app/Sourcetrail
	fi
else
	echo "no arguments: first argument 'release' or 'debug', second argument 'test' for tests"
fi
