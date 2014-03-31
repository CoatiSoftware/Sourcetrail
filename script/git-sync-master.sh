#!/bin/sh

ABORT="\033[31mAbort:\033[00m"
ALERT="\033[31mAlert:\033[00m"
SUCCESS="\033[32mSuccess:\033[00m"
INFO="\033[33mInfo:\033[00m"

BRANCH_NAME=$(git symbolic-ref -q --short HEAD)

# check if on branch
if [ -z $BRANCH_NAME ]
then
	echo -e $ABORT "You are not on any branch."
	exit 1
fi

# switch to master
echo -e $INFO "Switching to master"

git checkout -q master

if [ $? != 0 ]
then
	echo -e $ABORT "Switching to master failed."
	exit 1
fi

# check clean index
echo -e $INFO "Checking index"

if [[ -n $(git diff HEAD) ]]
then
	echo -e $ABORT "You have uncommited changes on master."
	git checkout -q "$BRANCH_NAME"
	exit 1
fi

# fetch remote changes
echo -e $INFO "Fetching changes"

git fetch -q origin master

if [ $? != 0 ]
then
	echo -e $ABORT "Fetching from origin master failed."
	git checkout -q "$BRANCH_NAME"
	exit 1
fi

if [[ -z $(git diff origin/master) ]]
then
	echo -e $SUCCESS "Your master is already up-to-date."
	git checkout -q "$BRANCH_NAME"
	exit 0
fi

# rebase master to origin/master
git rebase -q origin/master

if [ $? != 0 ]
then
	echo -e $ALERT "Rebasing on master caused conflicts. This should never happen. Follow the instructions above to resolve them then use 'git push origin master' to update the remote."
	exit 1
fi

git checkout -q "$BRANCH_NAME"

echo -e $SUCCESS "Updated master"
exit 0
