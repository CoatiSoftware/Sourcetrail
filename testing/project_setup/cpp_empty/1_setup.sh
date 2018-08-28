
MY_PATH=`dirname "$0"`

SRC_PATH=$MY_PATH/data
WORKING_COPY_PATH=$MY_PATH/working_copy

mkdir -p $WORKING_COPY_PATH

cp -ar $SRC_PATH/. $WORKING_COPY_PATH

echo "Setup Complete"
