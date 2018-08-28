
MY_PATH=`dirname "$0"`

MY_PATH="${MY_PATH//\\//}"

SRC_PATH=$MY_PATH/data
WORKING_COPY_PATH=$MY_PATH/working_copy
WORKING_COPY_SRC_PATH=$WORKING_COPY_PATH/src
CBD_PATH=$WORKING_COPY_PATH/Test.cbp

mkdir -p $WORKING_COPY_PATH

cp -ar $SRC_PATH/. $WORKING_COPY_PATH

sed -i "s|<source_path>|${WORKING_COPY_SRC_PATH}|g" ${CBD_PATH}

echo "Setup Complete"
