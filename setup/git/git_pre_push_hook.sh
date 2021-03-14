#!/bin/bash

current_branch=$(git symbolic-ref -q --short HEAD)

FAIL="\033[31mFail:\033[00m"
PASS="\033[32mPass:\033[00m"
INFO="\033[33mInfo:\033[00m"

# handle delete remote branches
while read local_ref lorem_sha remote_ref remote_sha
do
	if [ "$local_ref" == "(delete)" ]
	then
		if [ "$remote_ref" == "refs/heads/master" ]
		then
			echo -e $FAIL "seriously don't delete the master"
			exit 1
		else
			echo "delete remote branch $remote_ref"
			exit 0
		fi
	fi
done

exit 0
