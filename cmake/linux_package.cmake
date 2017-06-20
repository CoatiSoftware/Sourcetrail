
function(InstallQtModule module qtversion componentName)
	get_filename_component(realpath $ENV{QT_DIR}/lib/libQt5${module}.so.${qtversion} REALPATH)
	INSTALL(FILES
		${realpath}
		DESTINATION Sourcetrail/lib
		COMPONENT ${componentName}
		RENAME libQt5${module}.so.${qtversion}
	)
endfunction(InstallQtModule)

function(GetAndInstallLibrary libraryName componentName)
	execute_process(
		COMMAND ${CMAKE_CXX_COMPILER} -print-file-name=${libraryName}
		OUTPUT_VARIABLE LIB
		OUTPUT_STRIP_TRAILING_WHITESPACE
	)
	message(STATUS "lib: ${LIB}")

	get_filename_component(realPath ${LIB} REALPATH)
	string(REGEX MATCH "([^\/]*)$" fileName ${realPath})

	string(REGEX MATCH ".*\.so\.[^.]*" SHORTER ${fileName})
	if (NOT EXISTS ${realPath})
		message(WARNING "${libraryName} not found")
	else()
		INSTALL(FILES
			${realPath}
			DESTINATION Sourcetrail/lib
			COMPONENT ${componentName}
			RENAME ${SHORTER}
		)
	endif()
endfunction(GetAndInstallLibrary)

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
		DESTINATION Sourcetrail
		COMPONENT ${componentName}
		PATTERN "log/*" EXCLUDE
		PATTERN "data/src" EXCLUDE
		PATTERN "projects" EXCLUDE
		PATTERN "data/install" EXCLUDE
		PATTERN "ProjectSettings_template.xml" EXCLUDE
		PATTERN "ApplicationSettings_template.xml" EXCLUDE
	)

	INSTALL(FILES
		${CMAKE_SOURCE_DIR}/bin/app/data/gui/installer/EULA.txt
		COMPONENT ${componentName}
		DESTINATION Sourcetrail
	)

	InstallQt(5 ${componentName})

	GetAndInstallLibrary(libicui18n.so ${componentName})
	GetAndInstallLibrary(libicudata.so ${componentName})
	GetAndInstallLibrary(libicuuc.so ${componentName})
	GetAndInstallLibrary(libudev.so ${componentName})
	GetAndInstallLibrary(libpng.so ${componentName})
	GetAndInstallLibrary(libEGL.so ${componentName})
	GetAndInstallLibrary(libselinux.so ${componentName})
	GetAndInstallLibrary(libXrender.so ${componentName})
	#GetAndInstallLibrary(libstdc++.so ${componentName})
	GetAndInstallLibrary(libX11-xcb.so ${componentName})
	GetAndInstallLibrary(libXi.so ${componentName})
	GetAndInstallLibrary(libxcb.so ${componentName})
	GetAndInstallLibrary(libfontconfig.so ${componentName})
	GetAndInstallLibrary(libfreetype.so ${componentName})
	GetAndInstallLibrary(libXext.so ${componentName})
	GetAndInstallLibrary(libX11.so ${componentName})
	GetAndInstallLibrary(libudev.so ${componentName})
	GetAndInstallLibrary(libGL.so ${componentName})
	GetAndInstallLibrary(libxcb-dri2.so ${componentName})
	GetAndInstallLibrary(libxcb-xfixes.so ${componentName})
	GetAndInstallLibrary(libxcb-render.so ${componentName})
	GetAndInstallLibrary(libxcb-shape.so ${componentName})
	GetAndInstallLibrary(libgbm.so.1 ${componentName})
	GetAndInstallLibrary(libdrm.so ${componentName})
	GetAndInstallLibrary(libglapi.so ${componentName})
	GetAndInstallLibrary(libXdamage.so ${componentName})
	GetAndInstallLibrary(libXfixes.so ${componentName})
	GetAndInstallLibrary(libxcb-glx.so ${componentName})
	GetAndInstallLibrary(libXxf86vm.so ${componentName})
	GetAndInstallLibrary(libdrm.so ${componentName})
	GetAndInstallLibrary(libXau.so ${componentName})

	INSTALL(DIRECTORY
		$ENV{QT_DIR}/plugins/platforms
		DESTINATION Sourcetrail/lib
		COMPONENT ${componentName}
	)

	INSTALL(DIRECTORY ${CMAKE_SOURCE_DIR}/bin/app/user
		DESTINATION Sourcetrail
		COMPONENT ${componentName}
		PATTERN "ApplicationSettings.xml" EXCLUDE
		PATTERN "ApplicationSettings_for_package.xml" EXCLUDE
		PATTERN "ProjectSettings_template.xml" EXCLUDE
		PATTERN "ApplicationSettings_template.xml" EXCLUDE
		PATTERN "window_settings_for_package.ini" EXCLUDE
	)

endfunction(AddSharedToComponent)

# Add shared files to full and trial version
AddSharedToComponent(FULL)

INSTALL(DIRECTORY
	${CMAKE_SOURCE_DIR}/ide_plugins/
	DESTINATION Sourcetrail/plugin
	COMPONENT FULL
	PATTERN "vs" EXCLUDE
)

INSTALL(DIRECTORY
	${CMAKE_BINARY_DIR}/share
	DESTINATION Sourcetrail/setup
	COMPONENT FULL
)

INSTALL(FILES
	${CMAKE_SOURCE_DIR}/setup/Linux/README
	DESTINATION Sourcetrail
	COMPONENT FULL
)

INSTALL(FILES
	${CMAKE_SOURCE_DIR}/setup/Linux/sourcetrail-mime.xml
	DESTINATION Sourcetrail/setup/share/mime/packages
	COMPONENT FULL
)

INSTALL(FILES
	${CMAKE_SOURCE_DIR}/setup/Linux/sourcetrail.desktop
	DESTINATION Sourcetrail/setup/share/applications
	COMPONENT FULL
)

INSTALL(PROGRAMS
	${CMAKE_SOURCE_DIR}/setup/Linux/install.sh
	${CMAKE_SOURCE_DIR}/setup/Linux/deinstall.sh
	${CMAKE_SOURCE_DIR}/setup/Linux/removeConfigs.sh
	DESTINATION Sourcetrail/setup
	COMPONENT FULL
	)

INSTALL(PROGRAMS
	${CMAKE_SOURCE_DIR}/setup/Linux/Sourcetrail.sh
	DESTINATION Sourcetrail
	COMPONENT FULL
)

INSTALL(TARGETS
	${APP_PROJECT_NAME}
	DESTINATION Sourcetrail
	COMPONENT FULL
)

INSTALL(TARGETS
	${APP_INDEXER_NAME}
	DESTINATION Sourcetrail
	COMPONENT FULL
)

# SET(CPACK_GENERATOR "DEB;TGZ")
SET(CPACK_GENERATOR "TGZ")
SET(CPACK_ARCHIVE_COMPONENT_INSTALL "ON")

string(REGEX REPLACE "^([0-9]+)\\..*" "\\1" VERSION_MAJOR "${GIT_VERSION_NUMBER}")
string(REGEX REPLACE "^[0-9]+\\.([0-9]+).*" "\\1" VERSION_MINOR "${GIT_VERSION_NUMBER}")
#string(REGEX REPLACE "^[0-9]+\\.[0-9]+\\.([0-9]+).*" "\\1" VERSION_PATCH "${GIT_VERSION_NUMBER}")
#if(${GIT_VERSION_NUMBER} STREQUAL ${VERSION_PATCH})
	#SET(VERSIONA_PATCH 0)
#endif()
string(REGEX REPLACE "^[0-9]+\\.[0-9]+-([0-9]+).*" "\\1" VERSION_COMMIT "${GIT_VERSION_NUMBER}")

if(${GIT_VERSION_NUMBER} STREQUAL ${VERSION_COMMIT})
	set(VERSION_NUMBER "${VERSION_MAJOR}_${VERSION_MINOR}")
else()
	set(VERSION_NUMBER "${VERSION_MAJOR}_${VERSION_MINOR}_${VERSION_COMMIT}")
endif()

SET(CPACK_PACKAGE_NAME "Sourcetrail")
SET(CPACK_PACKAGING_INSTALL_PREFIX "")
SET(CPACK_INCLUDE_TOPLEVEL_DIRECTORY 0)
#SET(CPACK_TOPLEVEL_TAG "Sourcetrail")
SET(CPACK_PACKAGING_INSTALL_DIRECTORY "Sourcetrail")
SET(CPACK_PACKAGE_VERSION ${VERSION_NUMBER})
SET(CPACK_PACKAGE_VENDOR "Coati Software")
SET(CPACK_INSTALL_SCRIPT "${CMAKE_SOURCE_DIR}/cmake/pre_install_linux.cmake")
if(${CMAKE_SYSTEM_PROCESSOR} STREQUAL "x86_64")
	SET(CPACK_SYSTEM_NAME "${CMAKE_SYSTEM_NAME}_64bit")
else()
	SET(CPACK_SYSTEM_NAME "${CMAKE_SYSTEM_NAME}_32bit")
endif()
SET(CPACK_PACKAGE_FILE_NAME "Sourcetrail_${VERSION_NUMBER}_${CPACK_SYSTEM_NAME}")
SET(CPACK_STRIP_FILES "Sourcetrail/sourcetrail")
SET(CPACK_PACKAGE_CONTACT "astallinger@coati.io")

INCLUDE(CPack)

