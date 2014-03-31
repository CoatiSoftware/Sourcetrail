#!/bin/sh

ABORT="\033[31mAbort:\033[00m"
SUCCESS="\033[32mSuccess:\033[00m"
INFO="\033[33mInfo:\033[00m"

echo -e $INFO "Checking index"

# check clean index
if [[ -n $(git diff HEAD) ]]
then
	echo -e $ABORT "You have uncommited changes."
	exit 1
fi

BRANCH_NAME=$(git symbolic-ref -q --short HEAD)
PUBLISH_BRANCH_NAME=_publish_"$BRANCH_NAME"_

echo -e $INFO "Checking branch $BRANCH_NAME"

# check if on branch
if [ -z $BRANCH_NAME ]
then
	echo -e $ABORT "You are not on any branch."
	exit 1
fi

# check if on master
if [ $BRANCH_NAME == "master" ]
then
	echo -e $ABORT "You are on master. Use this script only from a feature branch."
	exit 1
fi

git checkout -q master
git remote update &>/dev/null

echo -e $INFO "Checking master"

# check if master is up to date
if [[ -n $(git diff origin/master) ]]
then
	echo -e $ABORT "Your master is not up-to-date. Run 'sync.sh' to update your master, then run 'git rebase master' to update branch $BRANCH_NAME."
	git checkout -q $BRANCH_NAME
	exit 1
fi

MASTER_HEAD_COMMIT=$(git rev-parse HEAD)
BASE_COMMIT=$(git merge-base $BRANCH_NAME master)

git checkout -q $BRANCH_NAME

BRANCH_HEAD_COMMIT=$(git rev-parse HEAD)

# check if feature has commits
if [ $BRANCH_HEAD_COMMIT == $MASTER_HEAD_COMMIT ]
then
	echo -e $ABORT "Your feature branch has no commits."
	exit 1
fi

# check if feature is not behind master
if [ $BASE_COMMIT != $MASTER_HEAD_COMMIT ]
then
	echo -e $ABORT "Your feature branch is behind master. Use 'git rebase master' to update your feature branch."
	exit 1
fi

# switch to temporary branch for squashing
git checkout -q -b $PUBLISH_BRANCH_NAME

echo -e $INFO "Squashing commits"
git reset --soft $BASE_COMMIT
git commit

if [ $? != 0 ]
then
	git checkout -q $BRANCH_NAME
	git branch -q -D $PUBLISH_BRANCH_NAME
	exit 1
fi

echo -e $INFO "Rebasing on master"
git checkout -q master
git rebase -q $PUBLISH_BRANCH_NAME
git branch -q -d $PUBLISH_BRANCH_NAME

echo -e $INFO "Uploading to origin master"
git push origin master

if [ $? != 0 ]
then
	git reset --hard -q HEAD^
	git checkout -q $BRANCH_NAME
	echo -e $ABORT "Pushing to origin master has failed, there are new remote changes. Run 'sync.sh' to update your master, then run 'git rebase master' to update branch $BRANCH_NAME."
	exit 1
fi

echo -e $SUCCESS "Published branch $BRANCH_NAME successfully"
exit 0
