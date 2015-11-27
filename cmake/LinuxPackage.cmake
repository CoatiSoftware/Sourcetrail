# execute_process(
#     COMMAND ${CMAKE_SOURCE_DIR}/setup/Linux/findAndCopyLibraries.sh
#     WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
# )

# INSTALL(CODE "
#         execute_process(
#         COMMAND upx --best ${CMAKE_SOURCE_DIR}/bin/app/Release/Coati)"
# )

INSTALL(DIRECTORY ${CMAKE_SOURCE_DIR}/bin/app/data
    DESTINATION .
    PATTERN "log/*" EXCLUDE
    PATTERN "projects/ignored" EXCLUDE
    PATTERN "data/src" EXCLUDE
    PATTERN "*.sqlite" EXCLUDE
    PATTERN "ApplicationSettings.xml" EXCLUDE
    PATTERN "ApplicationSettings_for_package.xml" EXCLUDE
    PATTERN "ProjectSettings_template.xml" EXCLUDE
    PATTERN "ApplicationSettings_template.xml" EXCLUDE
)

INSTALL(DIRECTORY ${CMAKE_SOURCE_DIR}/ide_plugins/vs
    DESTINATION plugin
    PATTERN "vs2012" EXCLUDE
)

INSTALL(DIRECTORY ${CMAKE_SOURCE_DIR}/ide_plugins/sublime_text
    DESTINATION plugin
)


INSTALL(FILES ${CMAKE_SOURCE_DIR}/bin/app/data/ApplicationSettings_for_package.xml
    DESTINATION data
    RENAME ApplicationSettings.xml
)

INSTALL(FILES ${CMAKE_SOURCE_DIR}/setup/Linux/coati.desktop
    DESTINATION .
)

INSTALL(FILES ${CMAKE_SOURCE_DIR}/bin/app/data/gui/icon/logo_1024_1024.png
    DESTINATION .
    RENAME coati.png
)

INSTALL(FILES ${CMAKE_SOURCE_DIR}/web/coati_manual.pdf
    DESTINATION .
)

INSTALL(DIRECTORY ${CMAKE_SOURCE_DIR}/setup/Linux/lib
    DESTINATION .
)

INSTALL(FILES
    ${CMAKE_SOURCE_DIR}/setup/Linux/libqxcb.so
    DESTINATION platforms
)
INSTALL(PROGRAMS
    ${CMAKE_SOURCE_DIR}/setup/Linux/Coati.sh
    DESTINATION .
)
# INSTALL(PROGRAMS ${CMAKE_SOURCE_DIR}/setup/Linux/coati
#         DESTINATION bin/
# )
# add the automatically determined parts of the RPATH
# which point to directories outside the build tree to the install RPATH
SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

INSTALL(TARGETS ${APP_PROJECT_NAME}
    DESTINATION .
)

# SET(CPACK_GENERATOR "DEB;TGZ")
SET(CPACK_GENERATOR "TGZ")
SET(CPACK_ARCHIVE_COMPONENT_INSTALL "ON")

string(REGEX REPLACE "^([0-9]+)\\..*" "\\1" VERSION_MAJOR "${GIT_VERSION_NUMBER}")
string(REGEX REPLACE "^[0-9]+\\.([0-9]+).*" "\\1" VERSION_MINOR "${GIT_VERSION_NUMBER}")
string(REGEX REPLACE "^[0-9]+\\.[0-9]+\\.([0-9]+).*" "\\1" VERSION_PATCH "${GIT_VERSION_NUMBER}")
set(VERSION_NUMBER "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")

SET(CPACK_PACKAGE_NAME "Coati")
SET(CPACK_PACKAGING_INSTALL_PREFIX "")
SET(CPACK_PACKAGE_VERSION ${VERSION_NUMBER})
SET(CPACK_PACKAGE_VENDOR "Coati Software")
# SET(CPACK_PACKAGE_VERSION ${GIT_VERSION_NUMBER})
SET(CPACK_PACKAGE_EXECUTABLES "Coati")
SET(CPACK_STRIP_FILES "coati/Coati")
SET(CPACK_PACKAGE_CONTACT "astallinger@coati.io")
MESSAGE(STATUS "${CMAKE_SYSTEM}")

INCLUDE(CPack)
