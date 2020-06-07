#!/bin/sh

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

cp -r ../../../build/Release/share/icons/ .

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

VERSION=$(cat build/Release/version.txt)
VERSION=${VERSION/./_}
VERSION=${VERSION/./_}
export VERSION

linuxdeployqt AppDir/usr/bin/sourcetrail_indexer -qmake=$Qt5_DIR/bin/qmake -ignore-glob=*python*
rm -f AppDir/AppRun

linuxdeployqt AppDir/usr/share/applications/sourcetrail.desktop -qmake=$Qt5_DIR/bin/qmake -ignore-glob=*python* -appimage

rename x86_64 Linux_64bit *.AppImage
rename - _ *.AppImage
rename - _ *.AppImage
rename . _ *.AppImage
rename . _ *.AppImage

cp -R setup/Linux/data/package/* AppDir/usr

mv AppDir/usr/ Sourcetrail
cp -R setup/Linux/data/package/* Sourcetrail

tar -czvf Sourcetrail_${VERSION}_Linux_64bit.tar.gz Sourcetrail

rm -rf AppDir
rm -rf Sourcetrail
