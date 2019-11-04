FROM toopher/centos-i386:centos6

MAINTAINER "Andreas Stallinger" <astallinger@coati.io>

WORKDIR /opt

RUN rpm --rebuilddb; yum install -y yum-plugin-ovl ca-certificates;

RUN yum -y install wget && \
wget https://copr.fedorainfracloud.org/coprs/mlampe/devtoolset-6/repo/\
epel-6/mlampe-devtoolset-6-epel-6.repo -O /etc/yum.repos.d/mlampe-devtoolset.repo && \
yum -y update && \
yum -y install devtoolset-6-gcc devtoolset-6-gcc-c++ devtoolset-6-binutils \
       wget tar bzip2 git libtool which fuse fuse-devel libpng-devel automake \
       glibc-headers libstdc++-devel gcc-c++ freetype-devel fontconfig-devel \
       ibxml2-devel libstdc++-devel libXrender-devel patch xcb-util-keysyms-devel \
       libXi-devel libudev-devel openssl-devel sqlite-devel \
       gperftools gperf libicu-devel boost-devel \
       libxslt-devel docbook-style-xsl.noarch python27 \
       xz mesa-libEGL-devel mesa-libGL-devel glib-devel \
       java-1.8.0-openjdk-devel ccache re2c ImageMagick vim lftp unzip && \
yum clean all

# CMake and Maven
RUN wget https://cmake.org/files/v3.8/cmake-3.8.2.tar.gz && \
tar xf cmake-3.8.2.tar.gz && \
cd cmake-3.8.2 && ./bootstrap && \
make && make install && \
cd .. && rm cmake-3.8.2.tar.gz && \
rm -rf cmake-3.8.2 && \
wget https://archive.apache.org/dist/maven/maven-3/3.5.0/binaries/apache-maven-3.5.0-bin.tar.gz && \
tar xzf apache-maven-3.5.0-bin.tar.gz && \
ln -s apache-maven-3.5.0 maven && \
rm apache-maven-3.5.0-bin.tar.gz

ENV M2_HOME=/opt/maven
ENV PATH=${M2_HOME}/bin:${PATH}

# Install Qt
ARG QVERSION_SHORT=5.10
ARG QTVERSION=5.10.1

# Install Qt
RUN mkdir -p /qt && cd /qt && \
wget http://download.qt.io/archive/qt/${QVERSION_SHORT}/${QTVERSION}/\
single/qt-everywhere-src-${QTVERSION}.tar.xz && \
tar xvf qt-everywhere-src-${QTVERSION}.tar.xz && \
ln -sf /opt/rh/devtoolset-6/root/usr/bin/g++ /usr/bin/g++ && \
ln -sf /opt/rh/devtoolset-6/root/usr/bin/c++ /usr/bin/c++ && \
cd /qt/qt-everywhere-src-${QTVERSION} && \
./configure -v \
-prefix /opt/qt${QTVERSION} \
-skip qtgamepad \
-platform linux-g++ \
-qt-pcre \
-qt-xcb \
-qt-xkbcommon \
-no-pch \
-xkb-config-root /usr/share/X11/xkb \
-no-use-gold-linker \
-release \
-no-compile-examples \
-confirm-license \
-opensource \
-nomake examples \
-nomake tests \
-skip sensors \
-skip webchannel \
-skip webengine \
-skip 3d \
-skip doc \
-skip multimedia \
-skip tools \
-skip connectivity \
-skip androidextras \
-skip canvas3d && \
make -j8 && \
make -j8 install && rm -Rf /qt

## set env
ENV CXX=g++ \
CC=gcc \
DEVTOOLSET=/opt/rh/devtoolset-6/root/usr/

RUN wget https://www.python.org/ftp/python/2.7.13/Python-2.7.13.tgz && \
tar xzf Python-2.7.13.tgz && \
cd Python-2.7.13 && \
./configure && \
make altinstall

## Install llvm
ARG LLVM_VERSION=8.0.0
RUN alias python=/usr/local/bin/python2.7 && mkdir -p /llvm && cd /llvm && \
wget http://llvm.org/releases/${LLVM_VERSION}/llvm-${LLVM_VERSION}.src.tar.xz && \
tar xf llvm-${LLVM_VERSION}.src.tar.xz && \
cd llvm-${LLVM_VERSION}.src && pwd && \
cd tools && pwd && \
wget http://llvm.org/releases/${LLVM_VERSION}/cfe-${LLVM_VERSION}.src.tar.xz && \
tar xf cfe-${LLVM_VERSION}.src.tar.xz && \
cd /llvm/llvm-${LLVM_VERSION}.src/projects && \
wget http://llvm.org/releases/${LLVM_VERSION}/compiler-rt-${LLVM_VERSION}.src.tar.xz && \
tar xf compiler-rt-${LLVM_VERSION}.src.tar.xz && \
. /opt/rh/devtoolset-6/enable && \
cd /llvm/llvm-${LLVM_VERSION}.src && mkdir -p build && cd build && \
cmake .. -DCMAKE_INSTALL_PREFIX=/opt/llvm/ \
-DCMAKE_BUILD_TYPE=Release \
-DLLVM_ENABLE_RTTI=ON \
&& make -j8 install && rm -Rf /llvm
#COPY scripts/install-llvm.sh /opt/
#RUN ./install-llvm.sh

## Boost
ARG BOOST_MAJOR=1
ARG BOOST_MINOR=68
ARG BOOST_PATCH=0
ENV BOOST_VERSION=${BOOST_MAJOR}.${BOOST_MINOR}.${BOOST_PATCH} \
BOOST_VERSION_UNDERSCORE=${BOOST_MAJOR}_${BOOST_MINOR}_${BOOST_PATCH}

RUN wget http://downloads.sourceforge.net/project/boost/boost/${BOOST_VERSION}/boost_${BOOST_VERSION_UNDERSCORE}.tar.gz && \
tar -xzf boost_${BOOST_VERSION_UNDERSCORE}.tar.gz && cd boost_${BOOST_VERSION_UNDERSCORE} && \
./bootstrap.sh --with-libraries=filesystem,program_options,system,date_time --prefix=/opt/boost && \
./b2 install cxxstd=14 --link=static --variant=release --threading=multi --runtime-link=static --cxxflags="-std=c++14 -fPIC" && \
cd .. && rm boost_${BOOST_VERSION_UNDERSCORE}.tar.gz && rm boost_${BOOST_VERSION_UNDERSCORE} -r

## Ninja
RUN git clone https://github.com/martine/ninja.git && \
cd ninja && \
git checkout release && \
./configure.py --bootstrap && \
mv ninja /usr/bin/ && \
cd .. && rm -rf ninja

#add user
RUN useradd builder

# Make sure the above SCLs are already enabled
#ENTRYPOINT ["/bin/bash", "-c", "source", "/opt/rh/devtoolset-6/enable"]
#CMD ["/bin/bash", "-c", "source", "/opt/rh/devtoolset-6/enable"]

RUN GCC_VERSION=$(g++ -dumpversion) && \
ln -s /opt/rh/devtoolset-6/root/usr/include/c++/${GCC_VERSION} /usr/include/c++/${GCC_VERSION} && \
ln -s /opt/rh/devtoolset-6/root/usr/lib/gcc/i686-redhat-linux/${GCC_VERSION} \
/usr/lib/gcc/i686-redhat-linux/${GCC_VERSION} && \
echo "source /opt/rh/devtoolset-6/enable" >> /etc/bashrc

ENV CXX=/opt/rh/devtoolset-6/root/usr/bin/g++ \
CC=/opt/rh/devtoolset-6/root/usr/bin/gcc \
JAVA_HOME=/usr/lib/jvm/java-openjdk \
LLVM_DIR=/opt/llvm \
BOOST_DIR=/opt/boost \
QT_DIR=/opt/qt${QTVERSION}

WORKDIR /home/builder
USER builder

