#!/bin/sh

SOURCE_DIR=$1
BUILD_DIR=$2
ICON=$SOURCE_DIR/bin/app/data/gui/icon/logo_1024_1024.png
HICOLOR_DIR=$BUILD_DIR/share/icons/hicolor

echo "sourcedir: $SOURCE_DIR"
echo "builddir : $BUILD_DIR"
echo "hicolor  : $HICOLOR_DIR"
echo "icon     : $ICON"

createIcon()
{
    mkdir -p $HICOLOR_DIR/$1/apps/
    convert $ICON -resize $1 $HICOLOR_DIR/$1/apps/sourcetrail.png
}

createIcon 512x512
createIcon 256x256
createIcon 128x128
createIcon 64x64
createIcon 48x48
