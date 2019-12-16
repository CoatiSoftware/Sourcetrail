#!/bin/sh -l

echo "Foo1"

pwd
echo "Foo2"

ls -la
echo "Foo3"

git status
echo "Foo4"

git tag -l
echo "Foo5"


echo "Foo6"

mkdir deploy
cp README.md deploy/
cp LICENSE.txt deploy/
cd deploy
ls

#./script/buildonly.sh all
