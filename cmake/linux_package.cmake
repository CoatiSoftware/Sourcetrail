
function(InstallQtModule module qtversion)
	get_filename_component(realpath $ENV{QT_DIR}/lib/libQt5${module}.so.${qtversion} REALPATH)
	INSTALL(FILES
		${realpath}
		DESTINATION Sourcetrail/lib
		RENAME libQt5${module}.so.${qtversion}
	)
endfunction(InstallQtModule)

function(GetAndInstallLibrary libraryName withoutVersion)
	execute_process(
		COMMAND ${CMAKE_CXX_COMPILER} -print-file-name=${libraryName}
		OUTPUT_VARIABLE LIB
		OUTPUT_STRIP_TRAILING_WHITESPACE
	)
	message(STATUS "lib: ${LIB}")

	get_filename_component(realPath ${LIB} REALPATH)
	string(REGEX MATCH "([^\/]*)$" fileName ${realPath})

	if (withoutVersion)
		string(REGEX MATCH ".*\.so" SHORTER ${fileName})
	else()
		string(REGEX MATCH ".*\.so\.[^.]*" SHORTER ${fileName})
	endif()
	if (NOT EXISTS ${realPath})
		message(WARNING "${libraryName} not found")
	else()
		INSTALL(FILES
			${realPath}
			DESTINATION Sourcetrail/lib
			RENAME ${SHORTER}
		)
	endif()
endfunction(GetAndInstallLibrary)

function(InstallQt qtversion)
	InstallQtModule(Gui ${qtversion})
	InstallQtModule(Core ${qtversion})
	InstallQtModule(Network ${qtversion})
	InstallQtModule(XcbQpa ${qtversion})
	InstallQtModule(Widgets ${qtversion})
	InstallQtModule(DBus ${qtversion})
	InstallQtModule(Svg ${qtversion})
endfunction(InstallQt)

function(AddSharedToComponent)
	INSTALL(DIRECTORY
		${CMAKE_SOURCE_DIR}/bin/app/data
		DESTINATION Sourcetrail
		PATTERN "install" EXCLUDE
		PATTERN "ProjectSettings_template.xml" EXCLUDE
		PATTERN "color_scheme_template.xml" EXCLUDE
		PATTERN "python/SourcetrailPythonIndexer" EXCLUDE
	)

	if (BUILD_PYTHON_LANGUAGE_PACKAGE)
		INSTALL(PROGRAMS
			${CMAKE_SOURCE_DIR}/bin/app/data/python/SourcetrailPythonIndexer
			DESTINATION Sourcetrail/data/python
		)
	endif()

    InstallQt(5)

	GetAndInstallLibrary(libicui18n.so 0)
	GetAndInstallLibrary(libicudata.so 0)
	GetAndInstallLibrary(libicuuc.so 0)
	GetAndInstallLibrary(libudev.so 0)
	GetAndInstallLibrary(libpng.so 0)
	GetAndInstallLibrary(libEGL.so 0)
	GetAndInstallLibrary(libselinux.so 0)
	GetAndInstallLibrary(libXrender.so 0)
	#GetAndInstallLibrary(libstdc++.so 0)
	GetAndInstallLibrary(libX11-xcb.so 0)
	GetAndInstallLibrary(libXi.so 0)
	GetAndInstallLibrary(libxcb.so 0)
	GetAndInstallLibrary(libfontconfig.so  0)
	GetAndInstallLibrary(libfreetype.so  0)
	GetAndInstallLibrary(libXext.so  0)
	GetAndInstallLibrary(libX11.so  0)
	GetAndInstallLibrary(libudev.so  0)
	GetAndInstallLibrary(libGL.so  0)
	GetAndInstallLibrary(libxcb-dri2.so  0)
	GetAndInstallLibrary(libxcb-xfixes.so  0)
	GetAndInstallLibrary(libxcb-render.so  0)
	GetAndInstallLibrary(libxcb-shape.so  0)
	GetAndInstallLibrary(libgbm.so.1  0)
	GetAndInstallLibrary(libdrm.so  0)
	GetAndInstallLibrary(libglapi.so  0)
	GetAndInstallLibrary(libXdamage.so  0)
	GetAndInstallLibrary(libXfixes.so  0)
	GetAndInstallLibrary(libxcb-glx.so  0)
	GetAndInstallLibrary(libXxf86vm.so  0)
	GetAndInstallLibrary(libdrm.so 0)
	GetAndInstallLibrary(libXau.so 0)
	GetAndInstallLibrary(libssl.so 1)
	GetAndInstallLibrary(libcrypto.so 1)

	INSTALL(DIRECTORY
		$ENV{QT_DIR}/plugins/platforms
		DESTINATION Sourcetrail/lib
	)

	INSTALL(DIRECTORY ${CMAKE_SOURCE_DIR}/bin/app/user
		DESTINATION Sourcetrail
		PATTERN "log/*" EXCLUDE
		PATTERN "ApplicationSettings.xml" EXCLUDE
		PATTERN "ApplicationSettings_template.xml" EXCLUDE
	)

endfunction(AddSharedToComponent)

AddSharedToComponent()

INSTALL(DIRECTORY
	${CMAKE_SOURCE_DIR}/ide_plugins/
	DESTINATION Sourcetrail/plugin
	PATTERN "vs" EXCLUDE
)

INSTALL(DIRECTORY
	${CMAKE_BINARY_DIR}/share
	DESTINATION Sourcetrail/setup
)

INSTALL(FILES
	${CMAKE_SOURCE_DIR}/setup/Linux/README
	DESTINATION Sourcetrail
)

INSTALL(FILES
	${CMAKE_SOURCE_DIR}/setup/Linux/sourcetrail-mime.xml
	DESTINATION Sourcetrail/setup/share/mime/packages
)

INSTALL(FILES
	${CMAKE_SOURCE_DIR}/setup/Linux/sourcetrail.desktop
	DESTINATION Sourcetrail/setup/share/applications
)

INSTALL(PROGRAMS
	${CMAKE_SOURCE_DIR}/setup/Linux/install.sh
	${CMAKE_SOURCE_DIR}/setup/Linux/uninstall.sh
	${CMAKE_SOURCE_DIR}/setup/Linux/resetPreferences.sh
	DESTINATION Sourcetrail
)

INSTALL(PROGRAMS
	${CMAKE_SOURCE_DIR}/setup/Linux/Sourcetrail.sh
	DESTINATION Sourcetrail
)

INSTALL(TARGETS
	${APP_PROJECT_NAME}
	DESTINATION Sourcetrail
)

INSTALL(TARGETS
	${APP_INDEXER_NAME}
	DESTINATION Sourcetrail
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
