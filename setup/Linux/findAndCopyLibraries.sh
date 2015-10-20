#!/bin/sh
mkdir -p setup/Linux/lib
ldd bin/app/Release/Coati | grep "=> /" | awk '{print $3}' | xargs -I '{}' cp -v '{}' setup/Linux/lib

# extra files needed and not copied with the lines above
cp /usr/lib/qt/plugins/platforms/libqxcb.so setup/Linux
cp /usr/lib/libQt5XcbQpa.so.5 setup/Linux/lib
cp /usr/lib/libQt5DBus.so.5 setup/Linux/lib
