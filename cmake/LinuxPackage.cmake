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

function(InstallQtModule module qtversion componentName)
	get_filename_component(realpath ${EXTERNAL_BUILD}/qt/lib/libQt5${module}.so.${qtversion} REALPATH)
	INSTALL(FILES
		${realpath}
		DESTINATION lib
		COMPONENT ${componentName}
		RENAME libQt5${module}.so.${qtversion}
	)
endfunction(InstallQtModule)

function(InstallQt qtversion componentName)
	InstallQtModule(Gui ${qtversion} ${componentName})
	InstallQtModule(Core ${qtversion} ${componentName})
	InstallQtModule(Network ${qtversion} ${componentName})
	InstallQtModule(XcbQpa ${qtversion} ${componentName})
	InstallQtModule(Widgets ${qtversion} ${componentName})
	InstallQtModule(DBus ${qtversion} ${componentName})
endfunction(InstallQt)

function(AddSharedToComponent componentName)
	INSTALL(DIRECTORY
		${CMAKE_SOURCE_DIR}/bin/app/data
		DESTINATION .
		COMPONENT ${componentName}
		PATTERN "log/*" EXCLUDE
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

	InstallQt(5 ${componentName})

	get_filename_component(realpath ${LIBSTDCPP} REALPATH)
	INSTALL(FILES
		#C++
		${realpath}
		DESTINATION lib
		COMPONENT ${componentName}
	)

	INSTALL(FILES
		${EXTERNAL_BUILD}/qt/plugins/platforms/libqxcb.so
		DESTINATION platforms
		COMPONENT ${componentName}
	)

	INSTALL(DIRECTORY ${CMAKE_SOURCE_DIR}/bin/app/user
		DESTINATION .
		COMPONENT ${componentName}
		PATTERN "ApplicationSettings.xml" EXCLUDE
		PATTERN "ApplicationSettings_for_package.xml" EXCLUDE
		PATTERN "ProjectSettings_template.xml" EXCLUDE
		PATTERN "ApplicationSettings_template.xml" EXCLUDE
		PATTERN "window_settings_for_package.ini" EXCLUDE
	)

	INSTALL(FILES ${CMAKE_SOURCE_DIR}/bin/app/user/window_settings_for_package.ini
		DESTINATION user
		COMPONENT ${componentName}
		RENAME window_settings.ini
	)
endfunction(AddSharedToComponent)

execute_process(
	COMMAND clang -print-file-name=libstdc++.so
	OUTPUT_VARIABLE LIBSTDCPP
	OUTPUT_STRIP_TRAILING_WHITESPACE
)
# Add shared files to full and trial version
AddSharedToComponent(FULL)
AddSharedToComponent(TRIAL)

INSTALL(DIRECTORY
	${CMAKE_SOURCE_DIR}/ide_plugins/sublime_text
	DESTINATION plugin
	COMPONENT FULL
)

INSTALL(DIRECTORY
		${CMAKE_SOURCE_DIR}/bin/app/data/projects
		DESTINATION user
		COMPONENT FULL
		PATTERN "*.coatidb" EXCLUDE
)

INSTALL(DIRECTORY
		${CMAKE_SOURCE_DIR}/bin/app/data/projects
		DESTINATION user
		COMPONENT TRIAL
)

INSTALL(FILES
	${CMAKE_SOURCE_DIR}/setup/Linux/coati.desktop
	${CMAKE_SOURCE_DIR}/setup/Linux/coati-mime.xml
	DESTINATION setup
	COMPONENT FULL
	)
INSTALL(PROGRAMS
	${CMAKE_SOURCE_DIR}/setup/Linux/install.sh
	${CMAKE_SOURCE_DIR}/setup/Linux/deinstall.sh
	${CMAKE_SOURCE_DIR}/setup/Linux/removeConfigs.sh
	DESTINATION setup
	COMPONENT FULL
	)

INSTALL(PROGRAMS
	${CMAKE_SOURCE_DIR}/setup/Linux/Coati_trial.sh
	DESTINATION .
	COMPONENT TRIAL
	)

INSTALL(PROGRAMS
	${CMAKE_SOURCE_DIR}/setup/Linux/Coati.sh
	DESTINATION .
	COMPONENT FULL
)

INSTALL(PROGRAMS
	${CMAKE_SOURCE_DIR}/bin/app/Release/Coati_upx
	DESTINATION .
	COMPONENT FULL
	RENAME Coati
)

INSTALL(PROGRAMS
	${CMAKE_SOURCE_DIR}/bin/app/Release/Coati_trial_upx
	DESTINATION .
	COMPONENT TRIAL
	RENAME Coati_trial
)

#INSTALL(TARGETS
	#${APP_PROJECT_NAME}
	#DESTINATION .
	#COMPONENT FULL
#)

#INSTALL(TARGETS
	#${TRIAL_PROJECT_NAME}
	#DESTINATION .
	#COMPONENT TRIAL
#)

# SET(CPACK_GENERATOR "DEB;TGZ")
SET(CPACK_GENERATOR "TGZ")
SET(CPACK_ARCHIVE_COMPONENT_INSTALL "ON")

string(REGEX REPLACE "^([0-9]+)\\..*" "\\1" VERSION_MAJOR "${GIT_VERSION_NUMBER}")
string(REGEX REPLACE "^[0-9]+\\.([0-9]+).*" "\\1" VERSION_MINOR "${GIT_VERSION_NUMBER}")
string(REGEX REPLACE "^[0-9]+\\.[0-9]+\\.([0-9]+).*" "\\1" VERSION_PATCH "${GIT_VERSION_NUMBER}")
if(${GIT_VERSION_NUMBER} STREQUAL ${VERSION_PATCH})
	SET(VERSIONA_PATCH 0)
endif()
string(REGEX REPLACE "^[0-9]+\\.[0-9]+\\.[0-9]+-([0-9]+).*" "\\1" VERSION_COMMIT "${GIT_VERSION_NUMBER}")
if(${GIT_VERSION_NUMBER} STREQUAL ${VERSION_COMMIT})
	set(VERSION_NUMBER "${VERSION_MAJOR}_${VERSION_MINOR}_${VERSION_PATCH}")
else()
	set(VERSION_NUMBER "${VERSION_MAJOR}_${VERSION_MINOR}_${VERSION_PATCH}_${VERSION_COMMIT}")
endif()

SET(CPACK_PACKAGE_NAME "Coati")
SET(CPACK_PACKAGING_INSTALL_PREFIX "")
SET(CPACK_INCLUDE_TOPLEVEL_DIRECTORY 1)
SET(CPACK_PACKAGING_INSTALL_DIRECTORY "Coati")
SET(CPACK_PACKAGE_VERSION ${VERSION_NUMBER})
SET(CPACK_PACKAGE_VENDOR "Coati Software")
SET(CPACK_INSTALL_SCRIPT "${CMAKE_SOURCE_DIR}/cmake/pre_install_linux.cmake")
SET(CPACK_SYSTEM_NAME "${CMAKE_SYSTEM_NAME}_${CMAKE_SYSTEM_PROCESSOR}")
SET(CPACK_PACKAGE_FILE_NAME "Coati_${VERSION_NUMBER}_${CPACK_SYSTEM_NAME}")
#SET(CPACK_PACKAGE_EXECUTABLES "Coati")
SET(CPACK_STRIP_FILES "coati/Coati")
SET(CPACK_PACKAGE_CONTACT "astallinger@coati.io")

INCLUDE(CPack)

