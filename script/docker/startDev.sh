#!/bin/sh

DIR="$( cd "$( dirname "$0" )" && pwd )"

xhost + > /dev/null
if docker ps -a | grep sourcetrail > /dev/null
then
	nvidia-docker start -i sourcetrail_dev
else
	./createImage.sh
fi
xhost - > /dev/null
