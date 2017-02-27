cmake_minimum_required(VERSION 3.1)

#set (CMAKE_VERBOSE_MAKEFILE ON)

set (BINARY_DIR ${CMAKE_BINARY_DIR})

include(cmake/add_files.cmake)
include(cmake/create_source_groups.cmake)
include(cmake/version.cmake)
include(cmake/version_setup.cmake)
include(cmake/licenses.cmake)
include(cmake/setWinXxBits.cmake)

# Variables --------------------------------------------------------------------

set(PROJECT_NAME Coati)

set(APP_PROJECT_NAME "${PROJECT_NAME}")
set(LIB_LICENSE_PROJECT_NAME "${PROJECT_NAME}_lib_license")
set(LIB_GUI_PROJECT_NAME "${PROJECT_NAME}_lib_gui")
set(LIB_CXX_PROJECT_NAME "${PROJECT_NAME}_lib_cxx")
set(LIB_JAVA_PROJECT_NAME "${PROJECT_NAME}_lib_java")
set(LIB_PROJECT_NAME "${PROJECT_NAME}_lib")
set(LICENSE_GENERATOR_PROJECT_NAME "${PROJECT_NAME}_license_generator")
set(TEST_PROJECT_NAME "${PROJECT_NAME}_test")

if (WIN32)
	set(PLATFORM_INCLUDE "includesWindows.h")
	# this adds the library path for VLD, must be done before the project target is created
	if(${WINXXBITS} STREQUAL "Win32")
		LINK_DIRECTORIES($ENV{VLD_DIR}/lib/Win32)
	else()
		LINK_DIRECTORIES($ENV{VLD_DIR}/lib/Win64)
	endif()
elseif (APPLE)
	set(PLATFORM_INCLUDE "includesMac.h")
else ()
	set(PLATFORM_INCLUDE "includesLinux.h")
endif ()

# Project ----------------------------------------------------------------------

project(${PROJECT_NAME})

# set Standard build type to Release
set(CMAKE_BUILD_TYPE_INIT "Release")

#RPATH
if(UNIX AND NOT APPLE)
	set(CMAKE_SKIP_BUILD_RPATH FALSE)
	set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
	set(CMAKE_INSTALL_RPATH "$ORIGIN/lib/:$$ORIGIN/lib/")
	include(cmake/external.cmake)
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_C_STANDARD 99)

# Settings ---------------------------------------------------------------------
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unknown-warning-option -fcolor-diagnostics")
endif()

# For debugging the release build on linux
#if (UNIX AND "${CMAKE_BUILD_TYPE}" STREQUAL "Release")
	#add_definitions(-fno-omit-frame-pointer)
#endif ()

# Clang ------------------------------------------------------------------------

if (UNIX AND APPLE)
	if ("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
		find_package(LLVM REQUIRED PATHS "$ENV{CLANG_DIR}/build_release/lib/cmake/llvm" NO_DEFAULT_PATH)
	else()
		find_package(LLVM REQUIRED PATHS "$ENV{CLANG_DIR}/build_debug/lib/cmake/llvm" NO_DEFAULT_PATH)
	endif()
elseif(UNIX)
	find_package(LLVM REQUIRED PATHS "$ENV{LLVM_DIR}/lib/cmake" )
else()
	if(${WINXXBITS} STREQUAL "Win32")
		find_package(LLVM REQUIRED PATHS "$ENV{CLANG_DIR}/build_win32/lib/cmake/llvm")
	else()
		find_package(LLVM REQUIRED PATHS "$ENV{CLANG_DIR}/build_win64/lib/cmake/llvm")
	endif()
endif()

find_package(CLANG REQUIRED PATHS "${CMAKE_SOURCE_DIR}/cmake" NO_DEFAULT_PATH)

add_definitions(${CLANG_DEFINITIONS})

# Boost ------------------------------------------------------------------------

if(UNIX AND NOT APPLE)
	set(BOOST_ROOT $ENV{BOOST_DIR})
elseif(UNIX AND APPLE)
	set(BOOST_ROOT $ENV{BOOST_159_DIR})
	set(Boost_NO_SYSTEM_PATHS ON)
else()
	if(${WINXXBITS} STREQUAL "Win32")
		set(BOOST_ROOT "$ENV{BOOST_159_DIR}/win32")
	else()
		set(BOOST_ROOT "$ENV{BOOST_159_DIR}/win64")
	endif()
	set(Boost_NO_SYSTEM_PATHS ON)
endif()
set(Boost_USE_MULTITHREAD ON)
set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_STATIC_RUNTIME OFF)
set(BOOST_LIBRARYDIR $ENV{BOOST_159_DIR})

find_package(Boost 1.59 COMPONENTS system program_options filesystem date_time REQUIRED)

# Botan -------------------------------------------------------------------------

set(BOTAN_VERSION "botan-1.11")

if(APPLE)
	find_library(BOTAN ${BOTAN_VERSION} $ENV{BOTAN_DIR}/release NO_DEFAULT_PATH)
	find_library(BOTAN_D ${BOTAN_VERSION} $ENV{BOTAN_DIR}/debug NO_DEFAULT_PATH)
elseif(UNIX)
	find_library(BOTAN ${BOTAN_VERSION} "$ENV{BOTAN_DIR}/lib")
else()
	if(${WINXXBITS} STREQUAL "Win32")
		find_library(BOTAN botan "$ENV{BOTAN_DIR}/win32/release")
		find_library(BOTAN_D botan "$ENV{BOTAN_DIR}/win32/debug")
	else()
		find_library(BOTAN botan "$ENV{BOTAN_DIR}/win64/release")
		find_library(BOTAN_D botan "$ENV{BOTAN_DIR}/win64/debug")
	endif()
endif()
message(STATUS "botanversion: ${BOTAN}")

if(${BOTAN} STREQUAL "BOTAN-NOTFOUND")
	message(FATAL_ERROR "Botan not found")
else()
	message(STATUS "Botan found")
endif()

if(UNIX AND NOT APPLE)
	find_path(BOTAN_INCLUDE_DIR NAMES botan/botan.h
		HINTS "$ENV{BOTAN_DIR}/include"
		PATH_SUFFIXES ${BOTAN_VERSION}
		DOC "The Botan include directory")
elseif(WIN32)
	if(${WINXXBITS} STREQUAL "Win32")
		set(BOTAN_INCLUDE_DIR "$ENV{BOTAN_DIR}/win32//build/include")
	else()
		set(BOTAN_INCLUDE_DIR "$ENV{BOTAN_DIR}/win64/build/include")
	endif()
else()
	set(BOTAN_INCLUDE_DIR "$ENV{BOTAN_DIR}/build/include")
endif()

if(NOT BOTAN_INCLUDE_DIR)
	message(FATAL_ERROR "Botan include dir not found")
endif()


# Lib License -------------------------------------------------------------------

if (UNIX)
	set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/lib/")
else ()
	foreach( OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES} )
		string( TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG )
		set( CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG} "${CMAKE_BINARY_DIR}/${OUTPUTCONFIG}/lib/")
	endforeach( OUTPUTCONFIG CMAKE_CONFIGURATION_TYPES )
endif ()

add_subdirectory(src/lib_license)

add_library(${LIB_LICENSE_PROJECT_NAME} ${LIB_LICENSE_FILES})

create_source_groups(${LIB_LICENSE_FILES})

set_property(
		TARGET ${LIB_LICENSE_PROJECT_NAME}
		PROPERTY INCLUDE_DIRECTORIES
		"${CMAKE_SOURCE_DIR}/src/lib_license"
)

target_include_directories(${LIB_LICENSE_PROJECT_NAME} SYSTEM
	PUBLIC
	${Boost_INCLUDE_DIRS}
	${BOTAN_INCLUDE_DIR}
)

if(UNIX)
	target_link_libraries(${LIB_LICENSE_PROJECT_NAME} ${BOTAN})
else()
	link_directories(optimized "$ENV{BOTAN_DIR}/release")
	link_directories(debug "$ENV{BOTAN_DIR}/debug")

	target_link_libraries(${LIB_LICENSE_PROJECT_NAME} debug ${BOTAN_D})
	target_link_libraries(${LIB_LICENSE_PROJECT_NAME} optimized ${BOTAN})
endif()


# Lib --------------------------------------------------------------------------

add_subdirectory(src/lib)
add_subdirectory(src/external)

set_source_files_properties(${EXTERNAL_FILES} PROPERTIES COMPILE_FLAGS "-w")
set_source_files_properties(${EXTERNAL_C_FILES} PROPERTIES COMPILE_FLAGS "-std=gnu89 -w")

add_library(${LIB_PROJECT_NAME} ${LIB_FILES} ${EXTERNAL_FILES} ${EXTERNAL_C_FILES})

create_source_groups(${LIB_FILES})
create_source_groups(${EXTERNAL_FILES})
create_source_groups(${EXTERNAL_C_FILES})

set_property(
	TARGET ${LIB_PROJECT_NAME}
	PROPERTY INCLUDE_DIRECTORIES
		"${CMAKE_SOURCE_DIR}/src/lib"
		"${CMAKE_SOURCE_DIR}/src/external"
		"${CMAKE_SOURCE_DIR}/src/lib_license"
		"${CMAKE_SOURCE_DIR}/src/lib_cxx"
		"${CMAKE_SOURCE_DIR}/src/lib_java"
		"${CMAKE_BINARY_DIR}/src/lib_license"
)

target_include_directories(${LIB_PROJECT_NAME} SYSTEM
	PUBLIC ${Boost_INCLUDE_DIR}
	"${CMAKE_SOURCE_DIR}/src/external"
)

target_link_libraries(${LIB_PROJECT_NAME} ${Boost_LIBRARIES} ${LIB_LICENSE_PROJECT_NAME})


# Lib Cxx -------------------------------------------------------------------

add_subdirectory(src/lib_cxx)

add_library(${LIB_CXX_PROJECT_NAME} ${LIB_CXX_FILES})

create_source_groups(${LIB_CXX_FILES})

set_property(
	TARGET ${LIB_CXX_PROJECT_NAME}
	PROPERTY INCLUDE_DIRECTORIES
		"${CMAKE_SOURCE_DIR}/src/lib_cxx"
		"${CMAKE_SOURCE_DIR}/src/lib"
)

target_include_directories(${LIB_CXX_PROJECT_NAME} SYSTEM
	PUBLIC ${LLVM_INCLUDE_DIRS}
	${CLANG_INCLUDE_DIRS}
	${Boost_INCLUDE_DIRS}
)

link_directories(${LLVM_LIBRARY_DIRS} ${CLANG_LIBRARY_DIRS} ${Boost_LIBRARY_DIRS})
llvm_map_components_to_libnames(REQ_LLVM_LIBS support core libdriver passes)

target_link_libraries(${LIB_CXX_PROJECT_NAME} ${CLANG_LIBRARIES} ${REQ_LLVM_LIBS})

if (WIN32)
	target_link_libraries(${LIB_CXX_PROJECT_NAME} version)
endif()

# Lib Java -------------------------------------------------------------------

find_package(JNI)

add_subdirectory(src/lib_java)

add_library(${LIB_JAVA_PROJECT_NAME} ${LIB_JAVA_FILES})

create_source_groups(${LIB_JAVA_FILES})

set_property(
	TARGET ${LIB_JAVA_PROJECT_NAME}
	PROPERTY INCLUDE_DIRECTORIES
		"${CMAKE_SOURCE_DIR}/src/lib_java"
		"${CMAKE_SOURCE_DIR}/src/lib"
)

target_include_directories(${LIB_JAVA_PROJECT_NAME} SYSTEM
	PUBLIC ${JNI_INCLUDE_DIRS}
	${Boost_INCLUDE_DIRS}
)

link_directories(${Boost_LIBRARY_DIRS})

add_custom_command(
	TARGET ${LIB_JAVA_PROJECT_NAME}
	PRE_BUILD
	COMMAND ${PROJECT_SOURCE_DIR}/script/update_java_indexer.sh
	COMMENT "updating java indexer jars"
)


# Lib Gui ----------------------------------------------------------------------

add_subdirectory(src/lib_gui)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

# Find the QtWidgets library
if(UNIX)
	set(CMAKE_PREFIX_PATH "$ENV{QT_DIR}/")

	if(APPLE)
		set(Qt5Widgets_DIR "$ENV{QT_DIR}/cmake/Qt5Widgets/")
		set(Qt5PrintSupport_DIR "$ENV{QT_DIR}/cmake/Qt5PrintSupport/")
	endif()

else()
	if(${WINXXBITS} STREQUAL "Win32")
		set(CMAKE_PREFIX_PATH "$ENV{QT_WIN32_DIR}/")
	else()
		set(CMAKE_PREFIX_PATH "$ENV{QT_WIN64_DIR}/")
	endif()
	set(Qt5Widgets_DIR "${CMAKE_PREFIX_PATH}/lib/cmake/Qt5Widgets")
	set(Qt5PrintSupport_DIR "${CMAKE_PREFIX_PATH}/lib/cmake/Qt5PrintSupport")
	set(Qt5Network_DIR "${CMAKE_PREFIX_PATH}/lib/cmake/Qt5Network")
endif()

find_package(Qt5Widgets)
find_package(Qt5PrintSupport)
find_package(Qt5Network)

if(Qt5Widgets_FOUND)
	MESSAGE(STATUS "Found Qt ${Qt5Widgets_VERSION_STRING}")
	# FIX: Qt was built with -reduce-relocations
	if (Qt5_POSITION_INDEPENDENT_CODE)
		SET(CMAKE_POSITION_INDEPENDENT_CODE ON)
	endif()
endif()

# target for running versionnumber script
# workaround for running customcommand (ninja dependency cycle)
add_custom_target(
	versionnumber ALL
)

add_library(${LIB_GUI_PROJECT_NAME} ${LIB_GUI_FILES} ${CMAKE_BINARY_DIR}/src/lib_gui/version.h)

# command for versioning script
add_custom_command(
	TARGET versionnumber
	PRE_BUILD
	COMMAND ${CMAKE_COMMAND} -DBINARY_DIR=${CMAKE_BINARY_DIR} -P ${CMAKE_SOURCE_DIR}/cmake/version.cmake
	BYPRODUCTS ${CMAKE_BINARY_DIR}/src/lib_gui/version.h
	DEPENDS ${LIB_GUI_PROJECT_NAME}
	WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
	COMMENT "check/update version number"
)
add_dependencies(${LIB_GUI_PROJECT_NAME} versionnumber)

create_source_groups(${LIB_GUI_FILES})

set_property(
	TARGET ${LIB_GUI_PROJECT_NAME}
	PROPERTY INCLUDE_DIRECTORIES
		"${CMAKE_SOURCE_DIR}/src/lib_gui"
		"${CMAKE_SOURCE_DIR}/src/lib"
		"${CMAKE_SOURCE_DIR}/src/lib_cxx"
		"${CMAKE_SOURCE_DIR}/src/lib_license"
		"${CMAKE_BINARY_DIR}/src/lib_gui"
		"${CMAKE_BINARY_DIR}/src/lib_license"
)

# include external header without warnings
target_include_directories(${LIB_GUI_PROJECT_NAME} SYSTEM
		PUBLIC "${CMAKE_SOURCE_DIR}/src/external"
		PUBLIC ${Boost_INCLUDE_DIRS}
)

# Use the Widgets module from Qt 5.
qt5_use_modules(${LIB_GUI_PROJECT_NAME} Widgets)
qt5_use_modules(${LIB_GUI_PROJECT_NAME} Network)

# configure platform specific include file
configure_file(
	"${PROJECT_SOURCE_DIR}/src/lib_gui/platform_includes/includes.h.in"
	"${PROJECT_BINARY_DIR}/src/lib_gui/includes.h"
)

#configure the versioning file
configure_file(
	${CMAKE_SOURCE_DIR}/cmake/version.h.in
	${CMAKE_BINARY_DIR}/src/lib_gui/version.h
)

# configure public key for the coati version
include(cmake/publicKey.cmake)

set(CMAKE_AUTOMOC OFF)

# App --------------------------------------------------------------------------

if (UNIX)
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/app/")
else ()
	foreach( OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES} )
		string( TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG )
		set( CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} "${CMAKE_BINARY_DIR}/${OUTPUTCONFIG}/app/")
	endforeach( OUTPUTCONFIG CMAKE_CONFIGURATION_TYPES )
endif ()

add_subdirectory(src/app)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

if (WIN32)
	file(WRITE ${CMAKE_BINARY_DIR}/Coati.rc
		"// Icon with lowest ID value placed first to ensure application icon\n"
		"// remains consistent on all systems.\n"
		"IDI_ICON1               ICON                    \"${CMAKE_BINARY_DIR}/coati.ico\"\n"
	)

	add_executable(${APP_PROJECT_NAME} ${APP_FILES} ${CMAKE_BINARY_DIR}/Coati.rc)

	# hide the console when running a release build.
	set_target_properties(${APP_PROJECT_NAME} PROPERTIES LINK_FLAGS_DEBUG "/SUBSYSTEM:CONSOLE")
	set_target_properties(${APP_PROJECT_NAME} PROPERTIES COMPILE_DEFINITIONS_DEBUG "_CONSOLE")
	set_target_properties(${APP_PROJECT_NAME} PROPERTIES LINK_FLAGS_RELWITHDEBINFO "/SUBSYSTEM:CONSOLE")
	set_target_properties(${APP_PROJECT_NAME} PROPERTIES COMPILE_DEFINITIONS_RELWITHDEBINFO "_CONSOLE")
	set_target_properties(${APP_PROJECT_NAME} PROPERTIES LINK_FLAGS_RELEASE "/ENTRY:\"mainCRTStartup\" /SUBSYSTEM:WINDOWS")
	set_target_properties(${APP_PROJECT_NAME} PROPERTIES LINK_FLAGS_MINSIZEREL "/SUBSYSTEM:WINDOWS")
else ()
	add_executable(${APP_PROJECT_NAME} ${APP_FILES})
endif ()

create_source_groups(${APP_FILES})

target_link_libraries(${APP_PROJECT_NAME} ${LIB_GUI_PROJECT_NAME} ${LIB_CXX_PROJECT_NAME} ${LIB_JAVA_PROJECT_NAME} ${LIB_PROJECT_NAME} ${LIB_LICENSE_PROJECT_NAME})

set_property(
	TARGET ${APP_PROJECT_NAME}
	PROPERTY INCLUDE_DIRECTORIES
		"${CMAKE_SOURCE_DIR}/src/app"
		"${CMAKE_SOURCE_DIR}/src/lib"
		"${CMAKE_SOURCE_DIR}/src/lib_gui"
		"${CMAKE_SOURCE_DIR}/src/lib_license"
		"${CMAKE_SOURCE_DIR}/src/lib_cxx"
		"${CMAKE_SOURCE_DIR}/src/lib_java"
		"${CMAKE_BINARY_DIR}/src/lib_gui"
		"${CMAKE_BINARY_DIR}/src/lib_license"
)

target_include_directories(${APP_PROJECT_NAME} SYSTEM
		PUBLIC "${CMAKE_SOURCE_DIR}/src/external"
		PUBLIC ${Boost_INCLUDE_DIRS}
)

# Use the Widgets module from Qt 5.
qt5_use_modules(${APP_PROJECT_NAME} Widgets)
qt5_use_modules(${APP_PROJECT_NAME} Network)

# add platform specific libraries
if (APPLE)
	find_library(CORE_FOUNDATION CoreFoundation)
	target_link_libraries(${APP_PROJECT_NAME} ${CORE_FOUNDATION})
endif ()


# MacOSX Bundle ----------------------------------------------------------------

if (APPLE)

	set(MACOSX_BUNDLE_NAME ${PROJECT_NAME})
	set(MACOSX_BINARY_NAME ${APP_PROJECT_NAME})

	set(MACOSX_DYNAMIC_LIBRARIES "")
	string(REPLACE ";" " " MACOSX_DYNAMIC_LIBRARIES "${MACOSX_DYNAMIC_LIBRARIES}")

	get_property(QT_CORE_PATH TARGET ${Qt5Core_LIBRARIES} PROPERTY LOCATION)
	get_filename_component(QT_CORE_PATH ${QT_CORE_PATH} REALPATH)

	get_property(QT_GUI_PATH TARGET ${Qt5Gui_LIBRARIES} PROPERTY LOCATION)
	get_filename_component(QT_GUI_PATH ${QT_GUI_PATH} REALPATH)

	get_property(QT_WIDGETS_PATH TARGET ${Qt5Widgets_LIBRARIES} PROPERTY LOCATION)
	get_filename_component(QT_WIDGETS_PATH ${QT_WIDGETS_PATH} REALPATH)

	get_property(QT_PRINT_PATH TARGET ${Qt5PrintSupport_LIBRARIES} PROPERTY LOCATION)
	get_filename_component(QT_PRINT_PATH ${QT_PRINT_PATH} REALPATH)

	get_property(QT_NETWORK_PATH TARGET ${Qt5Network_LIBRARIES} PROPERTY LOCATION)
	get_filename_component(QT_NETWORK_PATH ${QT_NETWORK_PATH} REALPATH)

	list(APPEND MACOSX_QT_FRAMEWORKS ${QT_CORE_PATH} ${QT_GUI_PATH} ${QT_WIDGETS_PATH} ${QT_PRINT_PATH} ${QT_NETWORK_PATH})
	string(REPLACE ";" " " MACOSX_QT_FRAMEWORKS "${MACOSX_QT_FRAMEWORKS}")

	set(MACOSX_QT_DIR "$ENV{QT_DIR}")

	configure_file(
		${PROJECT_SOURCE_DIR}/setup/MacOSX/bundle_install.sh.in
		${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/bundle_install.sh
		@ONLY
	)

	configure_file(
		${PROJECT_SOURCE_DIR}/setup/MacOSX/Info.plist.in
		${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/bundle_info.plist
		@ONLY
	)

endif ()

# License Generator  --------------------------------------------------------

add_subdirectory(src/license_generator)

if (UNIX)
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/license_generator/")
else ()
	foreach( OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES} )
		string( TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG )
		set( CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} "${CMAKE_BINARY_DIR}/${OUTPUTCONFIG}/license_generator/")
	endforeach( OUTPUTCONFIG CMAKE_CONFIGURATION_TYPES )
endif ()

add_executable(${LICENSE_GENERATOR_PROJECT_NAME} ${LICENSE_GENERATOR_FILES})

create_source_groups(${LICENSE_GENERATOR_FILES})

set_property(
		TARGET ${LICENSE_GENERATOR_PROJECT_NAME}
		PROPERTY INCLUDE_DIRECTORIES
		"${CMAKE_SOURCE_DIR}/src/lib_license"
)

target_include_directories(${LICENSE_GENERATOR_PROJECT_NAME} SYSTEM
		PUBLIC ${Boost_INCLUDE_DIRS}
)

if(UNIX AND NOT APPLE)
	target_link_libraries(${LICENSE_GENERATOR_PROJECT_NAME} pthread rt)
endif()

target_link_libraries(${LICENSE_GENERATOR_PROJECT_NAME} ${LIB_LICENSE_PROJECT_NAME} ${Boost_LIBRARIES} )

# CxxTest ----------------------------------------------------------------------

if (UNIX)
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/test/")
else ()
	foreach( OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES} )
		string( TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG )
		set( CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} "${CMAKE_BINARY_DIR}/${OUTPUTCONFIG}/test/")
	endforeach( OUTPUTCONFIG CMAKE_CONFIGURATION_TYPES )
endif ()

add_subdirectory(src/test)

set(CXXTEST_INCLUDE_DIR $ENV{CXX_TEST_DIR})
set(CXXTEST_PYTHON_TESTGEN_EXECUTABLE ${CXXTEST_INCLUDE_DIR}/bin/cxxtestgen)
set(TESTGEN_FILE ${CMAKE_CURRENT_BINARY_DIR}/test_main.cpp)

find_package(PythonInterp REQUIRED)

add_executable (${TEST_PROJECT_NAME} ${TESTGEN_FILE} ${TEST_FILES} )

create_source_groups(${TEST_FILES})

target_link_libraries(${TEST_PROJECT_NAME} ${LIB_CXX_PROJECT_NAME} ${LIB_JAVA_PROJECT_NAME} ${LIB_PROJECT_NAME} ${LIB_LICENSE_PROJECT_NAME} ${LIB_GUI_PROJECT_NAME})

find_package(CxxTest)
if (CXXTEST_FOUND)
	set_property(
		TARGET ${TEST_PROJECT_NAME}
		PROPERTY INCLUDE_DIRECTORIES
			"${CXXTEST_INCLUDE_DIR}"
			"${CMAKE_SOURCE_DIR}/src/lib"
			"${CMAKE_SOURCE_DIR}/src/lib_gui"
			"${CMAKE_SOURCE_DIR}/src/lib_cxx"
			"${CMAKE_SOURCE_DIR}/src/lib_java"
			"${CMAKE_SOURCE_DIR}/src/external"
			"${CMAKE_SOURCE_DIR}/src/lib_license"
			"${Boost_INCLUDE_DIRS}"
	)
endif (CXXTEST_FOUND)

if (WIN32)

	add_custom_command(
		TARGET ${TEST_PROJECT_NAME}
		PRE_BUILD
		COMMAND cd ${PROJECT_SOURCE_DIR} && ${PYTHON_EXECUTABLE} ${CXXTEST_PYTHON_TESTGEN_EXECUTABLE} --runner=ParenPrinter -o ${TESTGEN_FILE} ${TEST_FILES}
		COMMENT "Generating unittest code with cxxtestgen"
	)

	add_custom_command(
		TARGET ${TEST_PROJECT_NAME}
		POST_BUILD
		COMMAND cd $(ProjectDir)../../bin/test/\n$(OutDir)$(TargetName)$(TargetExt)
		COMMENT "Running unittest code"
	)

elseif (UNIX)

	add_custom_command(
		OUTPUT ${TESTGEN_FILE}
		PRE_BUILD
		COMMAND rm -f ${TESTGEN_FILE}
		COMMAND cd ${PROJECT_SOURCE_DIR} && ${PYTHON_EXECUTABLE} ${CXXTEST_PYTHON_TESTGEN_EXECUTABLE} --runner=ParenPrinter -o ${TESTGEN_FILE} ${TEST_FILES}
		COMMENT "Generating unittest code with cxxtestgen"
	)

	add_custom_target(gen DEPENDS ${TESTGEN_FILE})

	add_dependencies(${TEST_PROJECT_NAME} gen)

endif ()

# Visual Leak Detector ---------------------------------------------------------

if (WIN32)
	include_directories($ENV{VLD_DIR}/include)
endif ()

# Linux package
if(UNIX AND NOT APPLE)
	include(cmake/LinuxPackage.cmake)
endif  ()
