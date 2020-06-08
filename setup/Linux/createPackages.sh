#!/bin/sh

# index sample projects

BIN_PATH=build/Release/app/Sourcetrail
PROJECTS_PATH=bin/app/user/projects

./$BIN_PATH index --full $PROJECTS_PATH/tictactoe_cpp/tictactoe_cpp.srctrlprj
./$BIN_PATH index --full $PROJECTS_PATH/tictactoe_py/tictactoe_py.srctrlprj
./$BIN_PATH index --full $PROJECTS_PATH/tutorial/tutorial.srctrlprj
./$BIN_PATH index --full $PROJECTS_PATH/javaparser/javaparser.srctrlprj


# create AppDir

rm -rf AppDir

mkdir AppDir
cd AppDir

mkdir usr
cd usr

mkdir bin
mkdir lib
mkdir share

cd bin
cp ../../../build/Release/app/Sourcetrail sourcetrail
cp ../../../build/Release/app/sourcetrail_indexer .
cd ..

cd share

mkdir applications
cd applications
cp ../../../../setup/Linux/data/sourcetrail.desktop .
cd ..

mkdir mime
cd mime
mkdir packages
cd packages
cp ../../../../../setup/Linux/data/sourcetrail-mime.xml .
cd ../..

mkdir data
cd data
cp -R ../../../../bin/app/data/color_schemes .
cp -R ../../../../bin/app/data/syntax_highlighting_rules .
cp -R ../../../../bin/app/data/fonts .
cp -R ../../../../bin/app/data/gui .
cp -R ../../../../bin/app/data/java .
cp -R ../../../../bin/app/data/python .
cp -R ../../../../bin/app/data/fallback .
cp -R ../../../../bin/app/user/projects fallback/
cp -R ../../../../bin/app/data/license .
cp -R ../../../../bin/app/data/cxx .
cd ..

cd ../../..

# print file structure
# find AppDir | sed -e "s/[^-][^\/]*\// |/g" -e "s/|\([^ ]\)/|-\1/"


# create icons

ICON=AppDir/usr/share/data/gui/icon/logo_1024_1024.png
HICOLOR_DIR=AppDir/usr/share/icons/hicolor

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


# Version for filename

VERSION=$(cat build/Release/version.txt)
VERSION=${VERSION/./_}
VERSION=${VERSION/./_}
export VERSION


# create .AppImage

linuxdeployqt AppDir/usr/bin/sourcetrail_indexer -qmake=$Qt5_DIR/bin/qmake -ignore-glob=*python*
rm -f AppDir/AppRun

linuxdeployqt AppDir/usr/share/applications/sourcetrail.desktop -qmake=$Qt5_DIR/bin/qmake -ignore-glob=*python* -appimage

rename x86_64 Linux_64bit *.AppImage
rename - _ *.AppImage
rename - _ *.AppImage


# create .tar.gz

mv AppDir/usr/ Sourcetrail
cp -R setup/Linux/data/package/* Sourcetrail

tar -czvf Sourcetrail_${VERSION}_Linux_64bit.tar.gz Sourcetrail


# cleanup

rm -rf AppDir
rm -rf Sourcetrail
