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
CDB_PATH=$WORKING_COPY_PATH/compile_commands.json

mkdir -p $WORKING_COPY_PATH

cp -a $SRC_PATH/. $WORKING_COPY_PATH

echo "[" >> $CDB_PATH
echo "  {" >> $CDB_PATH
echo "    \"directory\": \"${WORKING_COPY_PATH}\"," >> $CDB_PATH
echo "    \"command\": \"clang-tool -fms-extensions -fms-compatibility -fms-compatibility-version=19 -isystem \\\"${WORKING_COPY_SRC_PATH}/include\\\"  -D _DEBUG  -D _MT  -D _DLL  -D WIN32  -D _WINDOWS  -D BUILD_TYPE=\\\"\\\"  -D QT_WIDGETS_LIB  -D QT_GUI_LIB  -D QT_CORE_LIB  -D QT_NETWORK_LIB  -D QT_WINEXTRAS_LIB  -D QT_SVG_LIB  -D CMAKE_INTDIR=\\\"Debug\\\"  -D _MBCS -std=c++14 \\\"${WORKING_COPY_SRC_PATH}/main.cpp\\\"\"" >> $CDB_PATH
echo "    \"file\": \"${WORKING_COPY_SRC_PATH}/main.cpp\"" >> $CDB_PATH
echo "  }" >> $CDB_PATH
echo "]" >> $CDB_PATH

echo "Setup Complete"
