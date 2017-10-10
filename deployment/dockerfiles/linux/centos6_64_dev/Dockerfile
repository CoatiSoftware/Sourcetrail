FROM coatisoftware/centos6_64_qt_llvm:qt591-llvm401

MAINTAINER "Andreas Stallinger" <astallinger@coati.io>

USER root

WORKDIR /opt

RUN yum install -y yum install xorg-x11-drv-nouveau.x86_64 firefox gdb xkeyboard-config p7zip && yum clean all

ENV QT_XKB_CONFIG_ROOT="/usr/share/X11/xkb:$QT_XKB_CONFIG_ROOT"
RUN export uid=0 gid=0

RUN dbus-uuidgen > /var/lib/dbus/machine-id && \
wget http://download.qt.io/official_releases/qtcreator/4.2/4.2.0/installer_source/linux_gcc_64_rhel66/qtcreator.7z && \
7za x qtcreator.7z -o/usr/local && \
rm qtcreator.7z

WORKDIR /home/builder
USER builder
CMD /usr/bin/firefox

