#!/bin/sh -l

whoami

ls -la

mkdir /home/builder/java_indexer/target
mkdir /home/builder/java_indexer/target/classes

./script/buildonly.sh all
