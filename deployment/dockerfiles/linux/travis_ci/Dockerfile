FROM coatisoftware/centos7_64_qt_llvm:qt5126-llvm900

# TODO: remove after full image rebuilt ------------------------------------
USER root

RUN yum -y install file

RUN wget https://github.com/probonopd/linuxdeployqt/releases/download/6/linuxdeployqt-6-x86_64.AppImage && \
chmod a+x linuxdeployqt-6-x86_64.AppImage && \
./linuxdeployqt-6-x86_64.AppImage --appimage-extract && \
cp -r squashfs-root/usr/ /opt/linuxdeployqt && \
chmod -R 755 /opt/linuxdeployqt && \
rm linuxdeployqt-6-x86_64.AppImage && rm -rf squashfs-root
ENV PATH=/opt/linuxdeployqt/bin:${PATH}

USER builder
# --------------------------------------------------------------------------

COPY entrypoint.sh /entrypoint.sh

ENTRYPOINT ["/entrypoint.sh"]
