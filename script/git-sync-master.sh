#!/bin/bash

ABORT="\033[31mAbort:\033[00m"
ALERT="\033[31mAlert:\033[00m"
SUCCESS="\033[32mSuccess:\033[00m"
INFO="\033[33mInfo:\033[00m"

BRANCH_NAME=$(git symbolic-ref -q --short HEAD)

# check if on branch
echo -e $INFO "Checking on branch"

if [ -z $BRANCH_NAME ]
then
	echo -e $ABORT "You are not on any branch."
	exit 1
fi

# check clean index
echo -e $INFO "Checking index"

if [[ -n $(git diff HEAD) ]]
then
	echo -e $ABORT "You have uncommited changes."
	exit 1
fi

# switch to master
if [ $BRANCH_NAME != 'master' ]
then
	echo -e $INFO "Switching to master"
	git checkout -q master

	if [ $? != 0 ]
	then
		echo -e $ABORT "Switching to master failed."
		exit 1
	fi
fi

# fetch remote changes
echo -e $INFO "Fetching changes"
git fetch origin master

if [ $? != 0 ]
then
	echo -e $ABORT "Fetching from origin master failed"
	git checkout -q "$BRANCH_NAME"
	exit 1
fi

# rebase master to origin/master
if [[ -z $(git diff origin/master) ]]
then
	echo -e $SUCCESS "Your master is up-to-date"
else
	echo -e $INFO "Rebasing to origin/master"
	git rebase origin/master

	if [ $? != 0 ]
	then
		echo -e $ALERT "Rebasing on master caused conflicts. This should never happen. Follow the instructions above to resolve them then use 'git push origin master' to update the remote."
		exit 1
	fi

	echo -e $SUCCESS "Updated master"
fi

if [ $BRANCH_NAME == 'master' ]
then
	exit 0
fi

# switch to branch
echo -e $INFO "Switching back to $BRANCH_NAME"
git checkout -q "$BRANCH_NAME"

if [ $? != 0 ]
then
	echo -e $ABORT "Switching back to $BRANCH_NAME failed."
	exit 1
fi

# rebase branch to master
echo -e $INFO "Rebasing $BRANCH_NAME to master"
git rebase master

if [ $? != 0 ]
then
	echo -e $ALERT "Rebasing $BRANCH_NAME to master caused conflicts. Follow the instructions above to resolve them."
	exit 1
else
	echo -e $SUCCESS "$BRANCH_NAME is now up-to-date."
fi

exit 0
