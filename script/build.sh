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
		cd bin/test && ../../build/Release/Release/test/Coati_test
	elif [ "$2" = "keygen" ]
	then
		#echo "release keygen"
		cd bin/license_generator && ../../build/Release/Release/license_generator/Coati_license_generator
	else
		#echo "release app"
		cd bin/app && ../../build/Release/Release/app/Coati
	fi
elif [ "$1" = "debug" ] || [ "$1" = "d" ]
then
	if [ "$2" = "test" ]
	then
		#echo "debug test"
		cd bin/test && ../../build/Debug/Debug/test/Coati_test
	elif [ "$2" = "keygen" ]
	then
		#echo "debug keygen"
		cd bin/license_generator && ../../build/Debug/Debug/license_generator/Coati_license_generator
	else
		#echo "debug app"
		cd bin/app && ../../build/Debug/Debug/app/Coati
	fi
else
	echo "no arguments: first argument 'release' or 'debug', second argument 'test' for tests"
fi
