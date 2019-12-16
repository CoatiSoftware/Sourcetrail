FROM centos:7

MAINTAINER "Andreas Stallinger" <astallinger@coati.io>

WORKDIR /opt

RUN rpm --rebuilddb; yum install -y yum-plugin-ovl ca-certificates;

RUN yum -y install centos-release-scl epel-release && \
yum -y install make wget devtoolset-7-gcc devtoolset-7-gcc-c++ devtoolset-7-binutils \
	wget tar bzip2 git libtool which fuse fuse-devel libpng-devel automake \
	glibc-headers libstdc++-devel gcc-c++ freetype-devel fontconfig-devel\
	libxml2-devel libstdc++-devel libXrender libXrender-devel patch xcb-util-keysyms xcb-util-keysyms-devel \
	libxcb libxcb-devel xcb-util xcb-util-devel xcb-util-wm xcb-util-wm-devel xcb-util xcb-util-devel \
	libxkbcommon libxkbcommon-devel libXi-devel libudev-devel.x86_64 openssl-devel sqlite-devel.x86_64 \
	gperftools.x86_64 gperf.x86_64 libicu-devel.x86_64 boost-devel.x86_64 \
	libxslt-devel.x86_64 python27.x86_64 \
	xz mesa-libEGL-devel mesa-libGL-devel glib-devel \
	java-1.8.0-openjdk-devel re2c ccache ImageMagick vim lftp unzip && \
yum clean all

# CMake
RUN wget https://cmake.org/files/v3.8/cmake-3.8.2-Linux-x86_64.tar.gz && \
tar xf cmake-3.8.2-Linux-x86_64.tar.gz && \
cp -R cmake-3.8.2-Linux-x86_64/* /usr/local/ && cmake --version && \
rm -rf cmake-3.8.2-Linux-x86_64 && \
rm cmake-3.8.2-Linux-x86_64.tar.gz

# Maven
RUN wget https://archive.apache.org/dist/maven/maven-3/3.5.0/binaries/apache-maven-3.5.0-bin.tar.gz && \
tar xzf apache-maven-3.5.0-bin.tar.gz && \
ln -s apache-maven-3.5.0 maven && \
rm apache-maven-3.5.0-bin.tar.gz

# Ninja
RUN git clone https://github.com/martine/ninja.git && \
cd ninja && \
git checkout release && \
./configure.py --bootstrap && \
mv ninja /usr/local/bin/ && \
cd .. && rm -rf ninja

ENV M2_HOME=/opt/maven \
JAVA_HOME=/usr/lib/jvm/java-openjdk \
DEVTOOLSET=/opt/rh/devtoolset-7/root/usr/
ENV PATH=${M2_HOME}/bin:${PATH}

RUN make --version
RUN cmake --version
RUN mvn --version
RUN ninja --version

