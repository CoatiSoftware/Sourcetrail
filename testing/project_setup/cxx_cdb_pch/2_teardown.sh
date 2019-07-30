#!/bin/bash

# get absolute path to parent directory of script
MY_PATH=`dirname "$0"`
if [[ "${MY_PATH}" != *\\* ]]
then
	cd $MY_PATH
	MY_PATH=`pwd`
fi
MY_PATH="${MY_PATH//\\//}"

WORKING_COPY_PATH=$MY_PATH/working_copy

rm -rf $WORKING_COPY_PATH

echo "Teardown Complete"
