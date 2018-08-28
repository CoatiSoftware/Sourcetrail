#!/bin/bash

# get absolute path to parent directory of script
MY_PATH=`dirname "$0"`
if [[ "${MY_PATH}" != *\\* ]]
then
	cd $MY_PATH
	MY_PATH=`pwd`
fi
MY_PATH="${MY_PATH//\\//}"

SRC_PATH=$MY_PATH/data
WORKING_COPY_PATH=$MY_PATH/working_copy
WORKING_COPY_SRC_PATH=$WORKING_COPY_PATH/src
CBP_PATH=$WORKING_COPY_PATH/Test.cbp

mkdir -p $WORKING_COPY_PATH

cp -a $SRC_PATH/. $WORKING_COPY_PATH

sed "s|<source_path>|${WORKING_COPY_SRC_PATH}|g" $MY_PATH/data/Test.cbp > ${CBP_PATH}

echo "Setup Complete"
