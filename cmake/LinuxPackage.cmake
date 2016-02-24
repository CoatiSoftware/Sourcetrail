# execute_process(
#     COMMAND ${CMAKE_SOURCE_DIR}/setup/Linux/findAndCopyLibraries.sh
#     WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
# )

#INSTALL(CODE
	#"execute_process(COMMAND \"${APP_PROJECT_NAME}
	#-d ${CMAKE_SOURCE_DIR}/bin/app/data/projects/tictactoe/tictactoe.coatiproject\")"
#)

#INSTALL(CODE
	#"execute_process(COMMAND ${APP_PROJECT_NAME}
	#-d ${CMAKE_SOURCE_DIR}/bin/app/data/projects/tutorial/tutorial.coatiproject)"
#)

function(AddSharedToComponent componentName)
	INSTALL(DIRECTORY
		${CMAKE_SOURCE_DIR}/bin/app/data
		DESTINATION .
		COMPONENT ${componentName}
		PATTERN "log/*" EXCLUDE
		PATTERN "projects/ignored" EXCLUDE
		PATTERN "data/src" EXCLUDE
		PATTERN "data/gui/installer" EXCLUDE
		PATTERN "data/install" EXCLUDE
		PATTERN "ApplicationSettings.xml" EXCLUDE
		PATTERN "ApplicationSettings_for_package.xml" EXCLUDE
		PATTERN "ProjectSettings_template.xml" EXCLUDE
		PATTERN "ApplicationSettings_template.xml" EXCLUDE
	)

	INSTALL(FILES ${CMAKE_SOURCE_DIR}/bin/app/user/ApplicationSettings_for_package.xml
		DESTINATION user
		COMPONENT ${componentName}
		RENAME ApplicationSettings.xml
	)

	INSTALL(FILES
		${CMAKE_SOURCE_DIR}/bin/app/data/gui/installer/EULA.txt
		COMPONENT ${componentName}
		DESTINATION .
	)

	INSTALL(FILES
		#Qt libs
		# FIXME need a better solution for that
		${EXTERNAL_BUILD}/qt/lib/libQt5Gui.so.5
		${EXTERNAL_BUILD}/qt/lib/libQt5Gui.so.5.5
		${EXTERNAL_BUILD}/qt/lib/libQt5Gui.so.5.5.1
		${EXTERNAL_BUILD}/qt/lib/libQt5Core.so.5
		${EXTERNAL_BUILD}/qt/lib/libQt5Core.so.5.5
		${EXTERNAL_BUILD}/qt/lib/libQt5Core.so.5.5.1
		${EXTERNAL_BUILD}/qt/lib/libQt5Network.so.5
		${EXTERNAL_BUILD}/qt/lib/libQt5Network.so.5.5
		${EXTERNAL_BUILD}/qt/lib/libQt5Network.so.5.5.1
		${EXTERNAL_BUILD}/qt/lib/libQt5XcbQpa.so.5
		${EXTERNAL_BUILD}/qt/lib/libQt5XcbQpa.so.5.5
		${EXTERNAL_BUILD}/qt/lib/libQt5XcbQpa.so.5.5.1
		${EXTERNAL_BUILD}/qt/lib/libQt5Widgets.so.5
		${EXTERNAL_BUILD}/qt/lib/libQt5Widgets.so.5.5
		${EXTERNAL_BUILD}/qt/lib/libQt5Widgets.so.5.5.1
		${EXTERNAL_BUILD}/qt/lib/libQt5DBus.so.5
		${EXTERNAL_BUILD}/qt/lib/libQt5DBus.so.5.5
		${EXTERNAL_BUILD}/qt/lib/libQt5DBus.so.5.5.1

		#C++
		${LIBSTDCPP}
		DESTINATION lib
		COMPONENT ${componentName}
	)

	INSTALL(FILES
		${EXTERNAL_BUILD}/qt/plugins/platforms/libqxcb.so
		DESTINATION platforms
		COMPONENT ${componentName}
	)

	INSTALL(DIRECTORY
		${CMAKE_SOURCE_DIR}/bin/app/data/projects
		DESTINATION user
		COMPONENT ${componentName}
	)

	INSTALL(DIRECTORY ${CMAKE_SOURCE_DIR}/bin/app/user
		DESTINATION .
		COMPONENT ${componentName}
		PATTERN "ApplicationSettings.xml" EXCLUDE
		PATTERN "ApplicationSettings_for_package.xml" EXCLUDE
		PATTERN "ProjectSettings_template.xml" EXCLUDE
		PATTERN "ApplicationSettings_template.xml" EXCLUDE
		PATTERN "window_settings.ini_for_package" EXCLUDE
	)
endfunction(AddSharedToComponent)

execute_process(
	COMMAND clang -print-file-name=libstdc++.so
	OUTPUT_VARIABLE LIBSTDCPP
	OUTPUT_STRIP_TRAILING_WHITESPACE
)
# Add shared files to full and trial version
AddSharedToComponent(FULLVERSION)
AddSharedToComponent(TRIALVERSION)

INSTALL(DIRECTORY
	${CMAKE_SOURCE_DIR}/ide_plugins/sublime_text
    DESTINATION plugin
	COMPONENT FULLVERSION
)

INSTALL(FILES
    ${CMAKE_SOURCE_DIR}/setup/Linux/coati.desktop
    ${CMAKE_SOURCE_DIR}/setup/Linux/application-x-coati.xml
    DESTINATION setup
	COMPONENT FULLVERSION
)

INSTALL(PROGRAMS
	${CMAKE_SOURCE_DIR}/setup/Linux/install.sh
	${CMAKE_SOURCE_DIR}/setup/Linux/deinstall.sh
	${CMAKE_SOURCE_DIR}/setup/Linux/removeConfigs.sh
	${CMAKE_SOURCE_DIR}/setup/Linux/Coati_trial.sh
    DESTINATION .
	COMPONENT TRIALVERSION
)

INSTALL(PROGRAMS
    ${CMAKE_SOURCE_DIR}/setup/Linux/Coati.sh
    DESTINATION .
	COMPONENT FULLVERSION
)

INSTALL(PROGRAMS
	${CMAKE_SOURCE_DIR}/bin/app/Release/Coati_upx
	DESTINATION .
	COMPONENT FULLVERSION
	RENAME Coati
)

INSTALL(PROGRAMS
	${CMAKE_SOURCE_DIR}/bin/app/Release/Coati_trial_upx
	DESTINATION .
	COMPONENT TRIALVERSION
	RENAME Coati_trial
)

# SET(CPACK_GENERATOR "DEB;TGZ")
SET(CPACK_GENERATOR "TGZ")
SET(CPACK_ARCHIVE_COMPONENT_INSTALL "ON")

string(REGEX REPLACE "^([0-9]+)\\..*" "\\1" VERSION_MAJOR "${GIT_VERSION_NUMBER}")
string(REGEX REPLACE "^[0-9]+\\.([0-9]+).*" "\\1" VERSION_MINOR "${GIT_VERSION_NUMBER}")
string(REGEX REPLACE "^[0-9]+\\.[0-9]+\\.([0-9]+).*" "\\1" VERSION_PATCH "${GIT_VERSION_NUMBER}")
set(VERSION_NUMBER "${VERSION_MAJOR}_${VERSION_MINOR}_${VERSION_PATCH}")

SET(CPACK_PACKAGE_NAME "Coati")
SET(CPACK_PACKAGING_INSTALL_PREFIX "")
SET(CPACK_PACKAGING_INSTALL_DIRECTORY "Coati")
SET(CPACK_PACKAGE_VERSION ${VERSION_NUMBER})
SET(CPACK_PACKAGE_VENDOR "Coati Software")
SET(CPACK_INSTALL_SCRIPT "${CMAKE_SOURCE_DIR}/cmake/pre_install_linux.cmake")
SET(CPACK_SYSTEM_NAME "${CMAKE_SYSTEM_NAME}_${CMAKE_SYSTEM_PROCESSOR}")
#SET(CPACK_PACKAGE_EXECUTABLES "Coati")
SET(CPACK_STRIP_FILES "coati/Coati")
SET(CPACK_PACKAGE_CONTACT "astallinger@coati.io")

INCLUDE(CPack)
