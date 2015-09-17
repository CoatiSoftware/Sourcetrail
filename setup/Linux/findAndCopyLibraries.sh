#!/bin/sh
mkdir -p setup/Linux/lib
ldd bin/app/Release/Coati | grep "=> /" | awk '{print $3}' | xargs -I '{}' cp -v '{}' setup/Linux/lib
