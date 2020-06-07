#!/bin/sh

echo "Are you sure want to delete your Sourcetrail config?"
printf 'enter [y/n] '
read ans
case ${ans:=y} in [yY]*) ;; *) exit ;; esac

rm -rf ~/.config/sourcetrail
