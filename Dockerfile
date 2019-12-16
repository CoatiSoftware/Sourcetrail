FROM coatisoftware/centos7_64_qt_llvm:qt5126-llvm900

COPY . /home/builder

ENTRYPOINT ["/home/builder/entrypoint.sh"]
