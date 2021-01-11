FROM coatisoftware/centos7_64_qt_llvm:qt5126-llvm1100

WORKDIR /home/builder

RUN mkdir .config && \
	cd .config && \
	mkdir sourcetrail

COPY ApplicationSettings.xml /home/builder/.config/sourcetrail/ApplicationSettings.xml

COPY entrypoint.sh /entrypoint.sh

ENTRYPOINT ["/entrypoint.sh"]
