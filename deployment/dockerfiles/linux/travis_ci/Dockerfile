FROM coatisoftware/centos7_64_qt_llvm:qt5126-llvm900

USER root

RUN yum -y install file

USER builder

COPY entrypoint.sh /entrypoint.sh

ENTRYPOINT ["/entrypoint.sh"]
