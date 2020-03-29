FROM coatisoftware/centos7_64_base

MAINTAINER "Eberhard Graether" <egraether@coati.io>

RUN make --version
RUN cmake --version
RUN mvn --version
RUN ninja --version

WORKDIR /opt

## set env
ARG QTVERSION_SHORT=5.12
ARG QTVERSION=5.12.6

ENV Qt5_DIR=/opt/qt${QTVERSION} \
Clang_DIR=/opt/llvm \
BOOST_ROOT=/opt/boost \
LD_LIBRARY_PATH=$Qt5_DIR/lib/

# Qt
RUN mkdir -p /qt && cd /qt && \
wget http://download.qt.io/archive/qt/${QTVERSION_SHORT}/${QTVERSION}/\
single/qt-everywhere-src-${QTVERSION}.tar.xz && \
tar xvf qt-everywhere-src-${QTVERSION}.tar.xz && \
ln -sf /opt/rh/devtoolset-7/root/usr/bin/g++ /usr/bin/g++ && \
ln -sf /opt/rh/devtoolset-7/root/usr/bin/c++ /usr/bin/c++ && \
cd /qt/qt-everywhere-src-${QTVERSION} && \
./configure -v \
-prefix /opt/qt${QTVERSION} \
-skip qtgamepad \
-platform linux-g++ \
-qt-pcre \
-qt-xcb \
-xkbcommon \
-no-pch \
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

# LLVM/Clang
ARG LLVM_VERSION=9.0.0
RUN mkdir -p /llvm && cd /llvm && \
wget http://llvm.org/releases/${LLVM_VERSION}/llvm-${LLVM_VERSION}.src.tar.xz && \
tar xvf llvm-${LLVM_VERSION}.src.tar.xz && \
cd llvm-${LLVM_VERSION}.src && \
cd tools && \
wget http://llvm.org/releases/${LLVM_VERSION}/cfe-${LLVM_VERSION}.src.tar.xz && \
tar xvf cfe-${LLVM_VERSION}.src.tar.xz && \
. /opt/rh/python27/enable && \
. /opt/rh/devtoolset-7/enable && \
python --version && \
cd /llvm/llvm-${LLVM_VERSION}.src && mkdir -p build && cd build && \
cmake .. -DCMAKE_INSTALL_PREFIX=/opt/llvm/ \
-DCMAKE_BUILD_TYPE=Release \
-DLLVM_ENABLE_RTTI=ON \
&& make -j8 install && rm -Rf /llvm

WORKDIR /opt

# Boost
ARG BOOST_MAJOR=1
ARG BOOST_MINOR=67
ARG BOOST_PATCH=0
ENV BOOST_VERSION=${BOOST_MAJOR}.${BOOST_MINOR}.${BOOST_PATCH} \
BOOST_VERSION_UNDERSCORE=${BOOST_MAJOR}_${BOOST_MINOR}_${BOOST_PATCH}

RUN wget http://downloads.sourceforge.net/project/boost/boost/${BOOST_VERSION}/boost_${BOOST_VERSION_UNDERSCORE}.tar.gz && \
tar -xzf boost_${BOOST_VERSION_UNDERSCORE}.tar.gz && cd boost_${BOOST_VERSION_UNDERSCORE} && \
./bootstrap.sh --with-libraries=filesystem,program_options,system,date_time --prefix=/opt/boost && \
./b2 install cxxstd=14 --link=static --variant=release --threading=multi --runtime-link=static --cxxflags="-std=c++14 -fPIC" && \
cd .. && rm boost_${BOOST_VERSION_UNDERSCORE}.tar.gz && rm boost_${BOOST_VERSION_UNDERSCORE} -r

# linuxdeployqt
RUN wget https://github.com/probonopd/linuxdeployqt/releases/download/6/linuxdeployqt-6-x86_64.AppImage && \
chmod a+x linuxdeployqt-6-x86_64.AppImage && \
./linuxdeployqt-6-x86_64.AppImage --appimage-extract && \
cp -r squashfs-root/usr/ /opt/linuxdeployqt && \
chmod -R 755 /opt/linuxdeployqt && \
rm linuxdeployqt-6-x86_64.AppImage && rm -rf squashfs-root
ENV PATH=/opt/linuxdeployqt/bin:${PATH}

#add user
RUN useradd -u 1000 builder
WORKDIR /home/builder
USER builder
