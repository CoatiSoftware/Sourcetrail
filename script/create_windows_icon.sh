MY_PATH=`dirname "$0"`

ICON_INPUT_PATH=$MY_PATH/../bin/app/data/gui/icon
ICON_BUILD_PATH=$ICON_INPUT_PATH/build

mkdir -p $ICON_BUILD_PATH

magick convert $ICON_INPUT_PATH/logo_1024_1024.png -define icon:auto-resize=256,128,64,48,40,32,24,16 $ICON_BUILD_PATH/sourcetrail.ico

cp $ICON_BUILD_PATH/sourcetrail.ico $MY_PATH/../build/win32
cp $ICON_BUILD_PATH/sourcetrail.ico $MY_PATH/../build/win64

rm -rf $ICON_BUILD_PATH
