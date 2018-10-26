Dockerfiles for Sourcetrail
===========================


Build docker images
-------------------

docker build -t <account>/<reponame>:<tag> <Dockerfile>


Upload docker images
--------------------

docker login 
docker push <account>/<reponame>:<tag>


Update Guide
============

# delete old docker images

$ docker image ls
$ docker rmi coatisoftware/centos6_64_qt_llvm:<tag>
$ docker rmi coatisoftware/centos6_32_qt_llvm:<tag>

# change library version numbers in docker files

# build and upload docker images with new <tag> e.g. qt591-llvm500:

$ cd deployment/dockerfiles/linux/centos6_64
$ docker build -t coatisoftware/centos6_64_qt_llvm:<tag> .

$ docker login
$ docker push coatisoftware/centos6_64_qt_llvm:<tag>

$ cd ../centos6_32
$ docker build -t coatisoftware/centos6_32_qt_llvm:<tag> .

$ docker push coatisoftware/centos6_32_qt_llvm:<tag>

# update <tag> of linux images in .gitlab-ci.yml

# commit and publish changes

