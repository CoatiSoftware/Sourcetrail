# execute_process(
#     COMMAND ${CMAKE_SOURCE_DIR}/setup/Linux/findAndCopyLibraries.sh
#     WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
# )

INSTALL(DIRECTORY ${CMAKE_SOURCE_DIR}/bin/app/data
        DESTINATION share/coati/.config/coati
        PATTERN "log/*" EXCLUDE
        PATTERN "projects" EXCLUDE
        PATTERN "src" EXCLUDE
        PATTERN "ApplicationSettings.xml" EXCLUDE
        PATTERN "ApplicationSettings_distr.xml" EXCLUDE
        PATTERN "ProjectSettings_template.xml" EXCLUDE
        PATTERN "ApplicationSettings_template.xml" EXCLUDE
)

INSTALL(DIRECTORY ${CMAKE_SOURCE_DIR}/bin/app/data/src/tictactoe
        DESTINATION share/coati/.config/coati/data/projects
        )

INSTALL(FILES ${CMAKE_SOURCE_DIR}/bin/app/data/projects/tictactoe.xml
        DESTINATION share/coati/.config/coati/data/projects/tictactoe
        )

INSTALL(FILES ${CMAKE_SOURCE_DIR}/bin/app/data/ApplicationSettings_distr.xml
        DESTINATION share/coati/.config/coati/data
        RENAME ApplicationSettings.xml)

INSTALL(FILES ${CMAKE_SOURCE_DIR}/setup/Linux/coati.desktop
        DESTINATION share/applications
)
INSTALL(FILES ${CMAKE_SOURCE_DIR}/bin/app/data/gui/icon/logo_1024_1024.png
        DESTINATION share/icons
        RENAME coati.png)

INSTALL(DIRECTORY ${CMAKE_SOURCE_DIR}/setup/Linux/lib
        DESTINATION share/coati
)
INSTALL(FILES
        ${CMAKE_SOURCE_DIR}/setup/Linux/libqxcb.so
        DESTINATION share/coati/platforms
)
INSTALL(PROGRAMS
        ${CMAKE_SOURCE_DIR}/setup/Linux/Coati.sh
        DESTINATION share/coati
)
INSTALL(PROGRAMS ${CMAKE_SOURCE_DIR}/setup/Linux/coati
        DESTINATION bin/
)
# add the automatically determined parts of the RPATH
# which point to directories outside the build tree to the install RPATH
SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

INSTALL(TARGETS ${APP_PROJECT_NAME}
        DESTINATION share/coati
)

SET(CPACK_GENERATOR "DEB;TGZ")
SET(CPACK_ARCHIVE_COMPONENT_INSTALL "ON")

# DEBIAN PACKAGE
SET(CPACK_DEBIAN_PACKAGE_MAINTAINER "Andreas Stallinger <astallinger@coati.io>")
SET(CPACK_DEBIAN_PACKAGE_ARCHITECTURE  "amd64")
SET(CPACK_PACKAGE_NAME "Coati")
SET(CPACK_DEBIAN_PACKAGE_HOMEPAGE "http://coati.io")
SET(CPACK_DEBIAN_PACKAGE_SECTION "devel")
SET(CPACK_PACKAGING_INSTALL_PREFIX "/usr/local")
SET(CPACK_PACKAGE_VERSION ${GIT_VERSION_NUMBER})
SET(CPACK_PACKAGE_EXECUTABLES "Coati")
SET(CPACK_STRIP_FILES "share/coati/Coati")

INCLUDE(CPack)
