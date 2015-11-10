#!/bin/bash

ABORT="\033[31mAbort:\033[00m"
ALERT="\033[31mAlert:\033[00m"
SUCCESS="\033[32mSuccess:\033[00m"
INFO="\033[33mInfo:\033[00m"

BRANCH_NAME=$(git symbolic-ref -q --short HEAD)

#set branch to sync with
if [[ $# -gt 0 ]]
then
	SYNC_BRANCH=$1
else
	SYNC_BRANCH="master"
fi
echo -e $INFO  "branch to commit: $SYNC_BRANCH"

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

# switch to $SYNC_BRANCH
if [ $BRANCH_NAME != '$SYNC_BRANCH' ]
then
	echo -e $INFO "Switching to $SYNC_BRANCH"
	git checkout -q $SYNC_BRANCH

	if [ $? != 0 ]
	then
		echo -e $ABORT "Switching to $SYNC_BRANCH failed."
		exit 1
	fi
fi

# fetch remote changes
echo -e $INFO "Fetching changes"
git fetch origin $SYNC_BRANCH --tags

if [ $? != 0 ]
then
	echo -e $ABORT "Fetching from origin $SYNC_BRANCH failed"
	git checkout -q "$BRANCH_NAME"
	exit 1
fi

# rebase $SYNC_BRANCH to origin/$SYNC_BRANCH
if [[ -z $(git diff origin/$SYNC_BRANCH) ]]
then
	echo -e $SUCCESS "Your $SYNC_BRANCH is up-to-date"
else
	echo -e $INFO "Rebasing to origin/$SYNC_BRANCH"
	git rebase origin/$SYNC_BRANCH

	if [ $? != 0 ]
	then
		echo -e $ALERT "Rebasing on $SYNC_BRANCH caused conflicts. This should never happen. Follow the instructions above to resolve them then use 'git push origin master' to update the remote."
		exit 1
	fi

	echo -e $SUCCESS "Updated $SYNC_BRANCH"
fi

if [ $BRANCH_NAME == '$SYNC_BRANCH' ]
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

# rebase branch to $SYNC_BRANCH
echo -e $INFO "Rebasing $BRANCH_NAME to $SYNC_BRANCH"
git rebase $SYNC_BRANCH

if [ $? != 0 ]
then
	echo -e $ALERT "Rebasing $BRANCH_NAME to $SYNC_BRANCH caused conflicts. Follow the instructions above to resolve them."
	exit 1
else
	echo -e $SUCCESS "$BRANCH_NAME is now up-to-date."
fi

exit 0
