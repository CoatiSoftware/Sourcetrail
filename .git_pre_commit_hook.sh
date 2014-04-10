#!/bin/bash

ABORT="\033[31mAbort:\033[00m"
SUCCESS="\033[32mSuccess:\033[00m"
INFO="\033[33mInfo:\033[00m"

BRANCH_NAME=$(git symbolic-ref -q --short HEAD)

if [ -z $BRANCH_NAME ]
then
	echo -e $ABORT "You are not on any branch."
	exit 1
fi

if [ $BRANCH_NAME == "master" ]
then
	echo -e $ABORT "Commiting to master is prohibited."
	exit 1
fi

exit 0
