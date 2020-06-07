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
