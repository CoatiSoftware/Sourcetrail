#!/bin/sh

# Define commit message template:
git config commit.template .git_commit_template.txt

# Enable git commandline colors:
git config color.ui true

echo "setup complete"
