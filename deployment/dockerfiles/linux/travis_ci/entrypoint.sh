#!/bin/sh -l

echo "building commit: $1"

wget https://github.com/probonopd/linuxdeployqt/releases/download/6/linuxdeployqt-6-x86_64.AppImage
chmod a+x linuxdeployqt-6-x86_64.AppImage
./linuxdeployqt-6-x86_64.AppImage --appimage-extract

git clone https://github.com/CoatiSoftware/Sourcetrail.git

cd Sourcetrail

git checkout $1

./script/buildonly.sh all

./script/buildonly.sh package

./script/build.sh release test

./setup/Linux/createAppDir.sh

export VERSION=$(cat build/Release/version.txt)

../squashfs-root/usr/bin/linuxdeployqt AppDir/usr/bin/sourcetrail_indexer -qmake=$Qt5_DIR/bin/qmake -ignore-glob=*python*
rm -f AppDir/AppRun
../squashfs-root/usr/bin/linuxdeployqt AppDir/usr/share/applications/sourcetrail.desktop -qmake=$Qt5_DIR/bin/qmake -ignore-glob=*python* -appimage

mkdir release
cp ./Sourcetrail*.tar.gz ./release/
cp ./Sourcetrail*.AppImage ./release/
