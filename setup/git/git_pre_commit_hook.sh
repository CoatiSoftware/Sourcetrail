#!/bin/bash

ABORT="\033[31mAbort:\033[00m"
SUCCESS="\033[32mSuccess:\033[00m"
INFO="\033[33mInfo:\033[00m"
FAIL="\033[31mFail:\033[00m"
PASS="\033[32mPass:\033[00m"

BRANCH_NAME=$(git symbolic-ref -q --short HEAD)

ROOTDIR=$(pwd)

# Determine current platform
PLATFORM='unknown'
if [ "$(uname)" == "Darwin" ]; then
	PLATFORM='MacOS'
elif [ "$(expr substr $(uname -s) 1 5)" == "Linux" ]; then
	PLATFORM='Linux'
elif [ "$(expr substr $(uname -s) 1 10)" == "MINGW32_NT" ]; then
	PLATFORM='Windows'
fi

function build {
	echo -e $INFO Building $1 \($2\)
	#cmake --build build/$2 --target $1 > /dev/null
	#output to commandline for debugging
	cmake --build build/$2 --target $1
	if [ $? -eq 0 ]
	then
		echo -e $PASS Building $1 \($2\) passed
	else
		echo -e $FAIL Building $1 \($2\) failed
		exit 1
	fi
}

function build_type {
	echo -e $INFO Starting with buildtype: $1
	if [ "$PLATFORM" == "Windows" ]
	then
		echo -e $INFO Builing the Coati VS Solution
		cmake --build build --config $1
		if [ $? -ne 0 ]
		then
			echo -e $FAIL At least one build or test failed, no push to $branch
			exit 1
		fi
	else
		build Coati $1
		build Coati_test $1
		build Coati_license_generator $1
		run_tests $1
	fi
}

function run_tests {
	echo -e $INFO Run $1 Tests

	cd bin/test
	echo $1/Coati_test
	$1/Coati_test
	if [ $? -eq 0 ]
	then
		echo -e $PASS $1 Tests passed
	else
		echo -e $FAIL $1 Tests failed
		exit 1
	fi
	cd $ROOTDIR
}

#testing before commiting to a publish branch
if [[ ${BRANCH_NAME:0:9} == "_publish_"  ]]
then
	echo Try to publish
	echo Run builds and test
	echo This will take a while
	build_type Release
	echo -e $PASS All builds and tests passed
	exit 0
fi

if [ -z $BRANCH_NAME ]
then
	echo -e $ABORT "You are not on any branch."
	exit 1
fi

if [ $BRANCH_NAME == "master" ]
then
	echo -e $ABORT "Commiting to master is prohibited."
	exit 1
fi

if [ $BRANCH_NAME == "public_beta" ]
then
	echo -e $ABORT "Commiting to public_beta is prohibited."
	exit 1
fi

exit 0
