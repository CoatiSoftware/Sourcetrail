FROM coatisoftware/centos7_64_qt_llvm:qt5126-llvm900

RUN yum -y install file

COPY entrypoint.sh /entrypoint.sh

ENTRYPOINT ["/entrypoint.sh"]
