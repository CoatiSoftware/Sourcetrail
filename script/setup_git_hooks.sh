#!/bin/bash

# get absolute path to parent directory of script
MY_PATH=`dirname "$0"`
if [[ "${MY_PATH}" != *\\* ]]
then
	cd $MY_PATH
	MY_PATH=`pwd`
fi
MY_PATH="${MY_PATH//\\//}"

cd ${MY_PATH}/..

git config commit.template setup/git/git_commit_template.txt
git config color.ui true
if [ -d ".git/hooks" ]; then
	cp setup/git/git_pre_commit_hook.sh .git/hooks/pre-commit
	cp setup/git/git_pre_push_hook.sh .git/hooks/pre-push
fi
