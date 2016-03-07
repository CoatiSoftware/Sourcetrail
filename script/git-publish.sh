#!/bin/bash

ABORT="\033[31mAbort:\033[00m"
SUCCESS="\033[32mSuccess:\033[00m"
INFO="\033[33mInfo:\033[00m"

echo -e $INFO "Checking index"

#set branch to commit to
if [[ $# -gt 0 ]]
then
	PUBLISH_TO_BRANCH=$1
else
	PUBLISH_TO_BRANCH="master"
fi
echo -e $INFO  "branch to commit: $PUBLISH_TO_BRANCH"

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

# check if on $PUBLISH_TO_BRANCH
if [ $BRANCH_NAME == $PUBLISH_TO_BRANCH ]
then
	echo -e $ABORT "You are on $PUBLISH_TO_BRANCH. Use this script only from a feature branch."
	exit 1
fi

git checkout -q $PUBLISH_TO_BRANCH
git remote update &>/dev/null

echo -e $INFO "Checking $PUBLISH_TO_BRANCH"

# check if $PUBLISH_TO_BRANCH is up to date
if [[ -n $(git diff origin/$PUBLISH_TO_BRANCH) ]]
then
	echo -e $ABORT "Your $PUBLISH_TO_BRANCH is not up-to-date. Run 'git-sync.sh' to update $PUBLISH_TO_BRANCH and rebase $BRANCH_NAME."
	git checkout -q $BRANCH_NAME
	exit 1
fi

PUBLIC_BRANCH_HEAD_COMMIT=$(git rev-parse HEAD)
BASE_COMMIT=$(git merge-base $BRANCH_NAME $PUBLISH_TO_BRANCH)

git checkout -q $BRANCH_NAME

BRANCH_HEAD_COMMIT=$(git rev-parse HEAD)

# check if feature has commits
if [ $BRANCH_HEAD_COMMIT == $PUBLIC_BRANCH_HEAD_COMMIT ]
then
	echo -e $ABORT "Your feature branch has no commits."
	exit 1
fi

# check if feature is not behind $PUBLIC_BRANCH_HEAD_COMMIT
if [ $BASE_COMMIT != $PUBLIC_BRANCH_HEAD_COMMIT ]
then
	echo -e $ABORT "Your feature branch is behind $PUBLISH_TO_BRANCH. Use 'git rebase $PUBLISH_TO_BRANCH' to update your feature branch."
	exit 1
fi

# Prepare publish message with commit messages
GIT_DIR=$(git rev-parse --show-toplevel)/.git
cat $GIT_DIR/../setup/git/git_commit_template.txt > $GIT_DIR/PUBLISH_MSG
printf "\n# commits:\n" >> $GIT_DIR/PUBLISH_MSG
git log --graph --format=%B $BASE_COMMIT..HEAD | sed 's/^/#   &/' >> $GIT_DIR/PUBLISH_MSG

# switch to temporary branch for squashing
git checkout -q -b $PUBLISH_BRANCH_NAME

echo -e $INFO "Squashing commits"
git reset --soft $BASE_COMMIT
git commit --template=$GIT_DIR/PUBLISH_MSG

if [ $? != 0 ]
then
	echo -e $ABORT "Commit was aborted."
	git checkout -q $BRANCH_NAME
	git branch -q -D $PUBLISH_BRANCH_NAME
	exit 1
fi

echo -e $INFO "Rebasing on $PUBLISH_TO_BRANCH"
git checkout -q $PUBLISH_TO_BRANCH
git rebase -q $PUBLISH_BRANCH_NAME
git branch -q -d $PUBLISH_BRANCH_NAME

echo -e $INFO "Uploading to origin $PUBLISH_TO_BRANCH"
git push origin $PUBLISH_TO_BRANCH

if [ $? != 0 ]
then
	git reset --hard -q HEAD^
	git checkout -q $BRANCH_NAME
	echo -e $ABORT "Pushing to origin $PUBLISH_TO_BRANCH has failed, there are new remote changes. Run 'git-sync.sh' to update $PUBLISH_TO_BRANCH and rebase $BRANCH_NAME."
	exit 1
fi

echo -e $SUCCESS "Published $BRANCH_NAME successfully"
exit 0
