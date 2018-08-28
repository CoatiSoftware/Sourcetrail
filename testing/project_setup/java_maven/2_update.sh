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

echo "" >> $WORKING_COPY_PATH/my-app/src/main/java/com/mycompany/app/Bar.java

echo "Update Complete"

