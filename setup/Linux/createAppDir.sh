#!/bin/sh

# find . | sed -e "s/[^-][^\/]*\// |/g" -e "s/|\([^ ]\)/|-\1/"

# wget https://github.com/probonopd/linuxdeployqt/releases/download/6/linuxdeployqt-6-x86_64.AppImage
# chmod a+x linuxdeployqt-6-x86_64.AppImage
# ./linuxdeployqt-6-x86_64.AppImage --appimage-extract

# git clone https://github.com/CoatiSoftware/Sourcetrail.git
# cd Sourcetrail
# ./script/buildonly.sh all

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
cp ../../../../setup/Linux/sourcetrail.desktop .
cd ..

cp -r ../../../build/Release/share/icons/ .

mkdir mime
cd mime
mkdir packages
cd packages
cp ../../../../../setup/Linux/sourcetrail-mime.xml .
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
cp -R ../../../../bin/app/data/license .
cp -R ../../../../bin/app/data/cxx .
cp -R ../../../../bin/app/user/projects .
cd ..

cd ../../..

# find AppDir | sed -e "s/[^-][^\/]*\// |/g" -e "s/|\([^ ]\)/|-\1/"

# ../squashfs-root/usr/bin/linuxdeployqt AppDir/usr/share/applications/sourcetrail.desktop -qmake=$Qt5_DIR/bin/qmake -ignore-glob=*python* -appimage
