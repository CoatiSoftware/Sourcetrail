#!/bin/sh -l

echo "whoami"
whoami

echo "ls -la"
ls -la

for var in "$@"
do
	echo $var
done

./script/buildonly.sh all
