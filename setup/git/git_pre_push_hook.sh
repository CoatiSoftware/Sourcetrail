#!/bin/bash

# This hook enforces to run builds and test when pushed to the $protected_branch

proteced_branches=("forced_tests" "master")

#proteced_branch='master'
current_branch=$(git symbolic-ref -q --short HEAD)

remote="$1"
url="$2"

FAIL="\033[31mFail:\033[00m"
PASS="\033[32mPass:\033[00m"
INFO="\033[33mInfo:\033[00m"

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
	cmake --build build/$2 --target $1 > /dev/null
	#output to commandline for debugging
	#cmake --build build/$2 --target $1
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
		cmake --build build --config $1 > /dev/null
		if [ $? -ne 0 ]
		then
			echo -e $FAIL At least one build or test failed, no push to $branch
			exit 1
		fi
	else
		build Coati $1
		build Coati_trial $1
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


#handle delete remote branches
while read local_ref lorem_sha remote_ref remote_sha
do
	if [ "$local_ref" == "(delete)" ]
	then
		if [ "$remote_ref" == "refs/heads/master" ]
		then
			echo "seriously dont delete the master"
			exit 1
		else
			echo "delete remote branch $remote_ref"
			exit 0
		fi
	fi
done

# handle push --tags
while read -d $'\0' arg ; do
    if [[ "$arg" == '--tags' ]] ; then
		echo "pushing tags"
		exit 0
    fi
done < /proc/$PPID/cmdline


for branch in "${proteced_branches[@]}"
do
	if [ $current_branch == $branch ]
	then
		echo Try to push to $branch
		echo Run builds and test
		echo This will take a while
#		build_type Debug
		build_type Release
		echo -e $PASS All builds and tests passed pushing to $branch
		exit 0
	fi
done

exit 0

