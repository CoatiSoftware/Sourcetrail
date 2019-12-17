#!/bin/sh -l

echo "building commit: $1"

git clone https://github.com/CoatiSoftware/Sourcetrail.git

cd Sourcetrail

git checkout $1

./script/buildonly.sh all

./script/buildonly.sh package

./script/build.sh release test
