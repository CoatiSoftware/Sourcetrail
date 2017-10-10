#!/bin/sh

QT_GRAPHICSSYSTEM="native" nvidia-docker run -ti \
-v ~/dev/sourcetrail_docker_64:/home/builder/sourcetrail \
-v /tmp/.X11-unix:/tmp/.X11-unix \
-e DISPLAY \
--privileged \
-u 0 \
--device=dev/dri:/dev/dri \
--name sourcetrail_dev \
coatisoftware/centos6_64_dev \
/bin/bash
