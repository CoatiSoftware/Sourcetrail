#!/bin/sh

BIN_PATH=build/Release/app/Sourcetrail
PROJECTS_PATH=bin/app/user/projects

./$BIN_PATH index --full $PROJECTS_PATH/tictactoe_cpp/tictactoe_cpp.srctrlprj
./$BIN_PATH index --full $PROJECTS_PATH/tictactoe_py/tictactoe_py.srctrlprj
./$BIN_PATH index --full $PROJECTS_PATH/tutorial/tutorial.srctrlprj
./$BIN_PATH index --full $PROJECTS_PATH/javaparser/javaparser.srctrlprj

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
cp -R ../../../../bin/app/user/projects fallback/
cp -R ../../../../bin/app/data/license .
cp -R ../../../../bin/app/data/cxx .
cd ..

cd ../../..

# find AppDir | sed -e "s/[^-][^\/]*\// |/g" -e "s/|\([^ ]\)/|-\1/"

export VERSION=$(cat build/Release/version.txt)

linuxdeployqt AppDir/usr/bin/sourcetrail_indexer -qmake=$Qt5_DIR/bin/qmake -ignore-glob=*python*
rm -f AppDir/AppRun
linuxdeployqt AppDir/usr/share/applications/sourcetrail.desktop -qmake=$Qt5_DIR/bin/qmake -ignore-glob=*python* -appimage

rename x86_64 Linux_64bit *.AppImage
rename - _ *.AppImage
rename - _ *.AppImage
rename . _ *.AppImage
rename . _ *.AppImage
