#!/bin/sh -l

who

ls -la

mkdir /home/builder/java_indexer/target
mkdir /home/builder/java_indexer/target/classes

./script/buildonly.sh all
