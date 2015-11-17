# execute_process(
#     COMMAND ${CMAKE_SOURCE_DIR}/setup/Linux/findAndCopyLibraries.sh
#     WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
# )

INSTALL(DIRECTORY ${CMAKE_SOURCE_DIR}/bin/app/data
        DESTINATION coati/data
        PATTERN "log/*" EXCLUDE
        PATTERN "projects/ingored" EXCLUDE
        PATTERN "src" EXCLUDE
        PATTERN "ApplicationSettings.xml" EXCLUDE
        PATTERN "ApplicationSettings_for_package.xml" EXCLUDE
        PATTERN "ProjectSettings_template.xml" EXCLUDE
        PATTERN "ApplicationSettings_template.xml" EXCLUDE
)

INSTALL(FILES ${CMAKE_SOURCE_DIR}/bin/app/data/ApplicationSettings_for_package.xml
        DESTINATION coati/data
        RENAME ApplicationSettings.xml)

INSTALL(FILES ${CMAKE_SOURCE_DIR}/setup/Linux/coati.desktop
        DESTINATION applications
)
INSTALL(FILES ${CMAKE_SOURCE_DIR}/bin/app/data/gui/icon/logo_1024_1024.png
        DESTINATION icons
        RENAME coati.png)

INSTALL(FILES ${CMAKE_SOURCE_DIR}/web/coati_manual.pdf
        DESTINATION coati)

INSTALL(DIRECTORY ${CMAKE_SOURCE_DIR}/setup/Linux/lib
        DESTINATION coati
)
INSTALL(FILES
        ${CMAKE_SOURCE_DIR}/setup/Linux/libqxcb.so
        DESTINATION coati/platforms
)
INSTALL(PROGRAMS
        ${CMAKE_SOURCE_DIR}/setup/Linux/Coati.sh
        DESTINATION coati
)
INSTALL(PROGRAMS ${CMAKE_SOURCE_DIR}/setup/Linux/coati
        DESTINATION bin/
)
# add the automatically determined parts of the RPATH
# which point to directories outside the build tree to the install RPATH
SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

INSTALL(TARGETS ${APP_PROJECT_NAME}
        DESTINATION coati
)

# SET(CPACK_GENERATOR "DEB;TGZ")
SET(CPACK_GENERATOR "TGZ")
SET(CPACK_ARCHIVE_COMPONENT_INSTALL "ON")

# DEBIAN PACKAGE
# SET(CPACK_DEBIAN_PACKAGE_MAINTAINER "Andreas Stallinger <astallinger@coati.io>")
# SET(CPACK_DEBIAN_PACKAGE_ARCHITECTURE  "amd64")
# SET(CPACK_DEBIAN_PACKAGE_HOMEPAGE "http://coati.io")
# SET(CPACK_DEBIAN_PACKAGE_SECTION "devel")

SET(CPACK_PACKAGE_NAME "Coati")
SET(CPACK_PACKAGING_INSTALL_PREFIX "")
SET(CPACK_PACKAGE_VERSION ${GIT_VERSION_NUMBER})
SET(CPACK_PACKAGE_EXECUTABLES "Coati")
SET(CPACK_STRIP_FILES "coati/Coati")

INCLUDE(CPack)
