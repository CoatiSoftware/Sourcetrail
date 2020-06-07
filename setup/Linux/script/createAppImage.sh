#!/bin/sh

source createAppDir.sh

export VERSION=$(cat build/Release/version.txt)

linuxdeployqt AppDir/usr/bin/sourcetrail_indexer -qmake=$Qt5_DIR/bin/qmake -ignore-glob=*python*
rm -f AppDir/AppRun
linuxdeployqt AppDir/usr/share/applications/sourcetrail.desktop -qmake=$Qt5_DIR/bin/qmake -ignore-glob=*python* -appimage

rename x86_64 Linux_64bit *.AppImage
rename - _ *.AppImage
rename - _ *.AppImage
rename . _ *.AppImage
rename . _ *.AppImage
