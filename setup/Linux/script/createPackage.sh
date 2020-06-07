#!/bin/sh

source createAppDir.sh

VERSION=$(cat build/Release/version.txt)
VERSION=${VERSION/./_}
VERSION=${VERSION/./_}

linuxdeployqt AppDir/usr/bin/sourcetrail_indexer -qmake=$Qt5_DIR/bin/qmake -ignore-glob=*python*
rm -f AppDir/AppRun

linuxdeployqt AppDir/usr/share/applications/sourcetrail.desktop -qmake=$Qt5_DIR/bin/qmake -ignore-glob=*python*
rm -f AppDir/AppRun

cp -R setup/Linux/data/package/* AppDir/usr

mv AppDir/usr/ Sourcetrail

tar -czvf Sourcetrail_${VERSION}_Linux_64bit.tar.gz Sourcetrail

rm -rf AppDir
rm -rf Sourcetrail
