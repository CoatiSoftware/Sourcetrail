MY_PATH=`dirname "$0"`

ICON_INPUT_PATH=$MY_PATH/../bin/app/data/gui/icon
ICON_SIZES_PATH=$ICON_INPUT_PATH/sizes
ICON_BUILD_PATH=$ICON_INPUT_PATH/build

mkdir -p $ICON_SIZES_PATH
mkdir -p $ICON_BUILD_PATH

convert $ICON_INPUT_PATH/logo_1024_1024.png -resize 16x16 $ICON_SIZES_PATH/icon_16.png
convert $ICON_INPUT_PATH/logo_1024_1024.png -resize 24x24 $ICON_SIZES_PATH/icon_24.png
convert $ICON_INPUT_PATH/logo_1024_1024.png -resize 32x32 $ICON_SIZES_PATH/icon_32.png
convert $ICON_INPUT_PATH/logo_1024_1024.png -resize 40x40 $ICON_SIZES_PATH/icon_40.png
convert $ICON_INPUT_PATH/logo_1024_1024.png -resize 48x48 $ICON_SIZES_PATH/icon_48.png
convert $ICON_INPUT_PATH/logo_1024_1024.png -resize 64x64 $ICON_SIZES_PATH/icon_64.png
convert $ICON_INPUT_PATH/logo_1024_1024.png -resize 128x128 $ICON_SIZES_PATH/icon_128.png
convert $ICON_INPUT_PATH/logo_1024_1024.png -resize 256x256 $ICON_SIZES_PATH/icon_256.png

convert $ICON_SIZES_PATH/icon_16.png $ICON_SIZES_PATH/icon_24.png $ICON_SIZES_PATH/icon_32.png $ICON_SIZES_PATH/icon_40.png $ICON_SIZES_PATH/icon_48.png $ICON_SIZES_PATH/icon_64.png $ICON_SIZES_PATH/icon_128.png $ICON_SIZES_PATH/icon_256.png -colors 256 $ICON_BUILD_PATH/sourcetrail.ico

cp $ICON_BUILD_PATH/sourcetrail.ico $MY_PATH/../build/win32
cp $ICON_BUILD_PATH/sourcetrail.ico $MY_PATH/../build/win64

rm -rf $ICON_SIZES_PATH
rm -rf $ICON_BUILD_PATH
