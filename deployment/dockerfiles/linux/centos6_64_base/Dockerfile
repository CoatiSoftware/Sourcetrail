FROM centos:6

MAINTAINER "Andreas Stallinger" <astallinger@coati.io>

WORKDIR /opt

RUN rpm --rebuilddb; yum install -y yum-plugin-ovl ca-certificates;

RUN yum -y install centos-release-scl epel-release && \
yum -y install wget devtoolset-4-gcc devtoolset-4-gcc-c++ devtoolset-4-binutils \
	wget tar bzip2 git libtool which fuse fuse-devel libpng-devel automake \
	glibc-headers libstdc++-devel gcc-c++ freetype-devel fontconfig-devel\
	libxml2-devel libstdc++-devel libXrender-devel patch xcb-util-keysyms-devel \
	libXi-devel libudev-devel.x86_64 openssl-devel sqlite-devel.x86_64 \
	gperftools.x86_64 gperf.x86_64 libicu-devel.x86_64 boost-devel.x86_64 \
	libxslt-devel.x86_64 python27.x86_64 \
	xz mesa-libEGL-devel mesa-libGL-devel glib-devel git19 \
	java-1.8.0-openjdk-devel re2c ccache ImageMagick vim lftp && \
yum clean all

# CMake
RUN wget https://cmake.org/files/v3.8/cmake-3.8.2-Linux-x86_64.tar.gz && \
tar xf cmake-3.8.2-Linux-x86_64.tar.gz && \
cd cmake-3.8.2-Linux-x86_64 && cp -R * / && \
cd .. && rm -rf cmake-3.8.2-Linux-x86_64 && \
rm cmake-3.8.2-Linux-x86_64.tar.gz && \
wget https://archive.apache.org/dist/maven/maven-3/3.5.0/binaries/apache-maven-3.5.0-bin.tar.gz && \
tar xzf apache-maven-3.5.0-bin.tar.gz && \
ln -s apache-maven-3.5.0 maven && \
rm apache-maven-3.5.0-bin.tar.gz

ENV M2_HOME=/opt/maven \
JAVA_HOME=/usr/lib/jvm/java-openjdk \
DEVTOOLSET=/opt/rh/devtoolset-4/root/usr/
ENV PATH=${M2_HOME}/bin:${PATH}

# Make sure the above SCLs are already enabled
ENTRYPOINT ["/usr/bin/scl", "enable", "python27", "devtoolset-4", "git19", "--"]
CMD ["/usr/bin/scl", "enable", "python27", "devtoolset-4", "git19", "--", "/bin/bash"]

RUN GCC_VERSION=$(g++ -dumpversion) && \
ln -s /opt/rh/devtoolset-4/root/usr/include/c++/${GCC_VERSION} /usr/include/c++/${GCC_VERSION} && \
ln -s /opt/rh/devtoolset-4/root/usr/lib/gcc/x86_64-redhat-linux/${GCC_VERSION} \
/usr/lib/gcc/x86_64-redhat-linux/${GCC_VERSION}

