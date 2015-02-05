cmake_minimum_required(VERSION 2.8.9)

include(cmake/add_files.cmake)
include(cmake/create_source_groups.cmake)


# Variables --------------------------------------------------------------------

set(PROJECT_NAME masterproject)

set(APP_PROJECT_NAME "${PROJECT_NAME}_app")
set(LIB_PROJECT_NAME "${PROJECT_NAME}_lib")
set(TEST_PROJECT_NAME "${PROJECT_NAME}_test")

if (WIN32)
	set(PLATFORM_INCLUDE "includesWindows.h")
	LINK_DIRECTORIES($ENV{VLD_DIR}/lib/Win32) # this adds the library path for VLD, must be done before the project target is created
elseif (APPLE)
	set(PLATFORM_INCLUDE "includesMac.h")
else ()
	set(PLATFORM_INCLUDE "includesDefault.h")
endif ()

# Settings ---------------------------------------------------------------------

if (UNIX)
	add_definitions(-std=c++11 -Wno-unknown-warning-option -fcolor-diagnostics)
endif ()

# Project ----------------------------------------------------------------------

project(${PROJECT_NAME})

# Clang ------------------------------------------------------------------------

find_package(LLVM REQUIRED PATHS "$ENV{CLANG_DIR}/build/share/llvm/cmake")
find_package(CLANG REQUIRED PATHS "${CMAKE_SOURCE_DIR}/cmake")

add_definitions(${CLANG_DEFINITIONS})

# Boost ------------------------------------------------------------------------

set(BOOST_ROOT $ENV{BOOST_155_DIR})
set(Boost_USE_MULTITHREAD ON)
set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_STATIC_RUNTIME OFF)
set(BOOST_LIBRARYDIR $ENV{BOOST_155_DIR})
find_package(Boost 1.55 COMPONENTS system filesystem date_time REQUIRED)

# Eigen -------------------------------------------------------------------------

set(EIGEN_ROOT $ENV{EIGEN_DIR})

# Lib --------------------------------------------------------------------------

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/lib)

add_subdirectory(src/lib)
add_subdirectory(src/external)

set_source_files_properties(${CLANG_FILES} PROPERTIES COMPILE_FLAGS "-fno-rtti")

add_library(${LIB_PROJECT_NAME} ${LIB_FILES} ${CLANG_FILES} ${EXTERNAL_FILES})

create_source_groups(${LIB_FILES})
create_source_groups(${CLANG_FILES})
create_source_groups(${EXTERNAL_FILES})

set_target_properties(
	${LIB_PROJECT_NAME}
	PROPERTIES
		DEBUG_OUTPUT_NAME ${PROJECT_NAME}_d
		RELEASE_OUTPUT_NAME ${PROJECT_NAME}
)

set_property(
	TARGET ${LIB_PROJECT_NAME}
	PROPERTY INCLUDE_DIRECTORIES
		"${CMAKE_SOURCE_DIR}/src/lib"
		"${CMAKE_SOURCE_DIR}/src/external"
		${LLVM_INCLUDE_DIRS}
		${CLANG_INCLUDE_DIRS}
		${Boost_INCLUDE_DIRS}
		${EIGEN_ROOT}
)

link_directories(${LLVM_LIBRARY_DIRS} ${CLANG_LIBRARY_DIRS} ${Boost_LIBRARY_DIRS})

target_link_libraries(${LIB_PROJECT_NAME} ${CLANG_LIBRARIES} ${LLVM_AVAILABLE_LIBS} ${Boost_LIBRARIES})

# App --------------------------------------------------------------------------

if (UNIX)
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/app/${CMAKE_BUILD_TYPE})
else ()
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/app/)
endif ()

add_subdirectory(src/app)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

# Find the QtWidgets library
set(CMAKE_PREFIX_PATH "$ENV{QT_DIR}/")

find_package(Qt5Widgets)
find_package(Qt5PrintSupport)

add_executable(${APP_PROJECT_NAME} ${APP_FILES})

create_source_groups(${APP_FILES})

target_link_libraries(${APP_PROJECT_NAME} ${LIB_PROJECT_NAME})

set_property(
	TARGET ${APP_PROJECT_NAME}
	PROPERTY INCLUDE_DIRECTORIES
		"${CMAKE_SOURCE_DIR}/src/lib"
		"${CMAKE_SOURCE_DIR}/src/app"
		"${CMAKE_SOURCE_DIR}/build/src/app"
		"${CMAKE_SOURCE_DIR}/src/external"
)

# Use the Widgets module from Qt 5.
qt5_use_modules(${APP_PROJECT_NAME} Widgets)

# configure platform specific include file
configure_file(
	"${PROJECT_SOURCE_DIR}/src/app/platform_includes/includes.h.in"
	"${PROJECT_SOURCE_DIR}/build/src/app/includes.h"
)

# add platform specific libraries
if (APPLE)
	find_library(CORE_FOUNDATION CoreFoundation)
	target_link_libraries(${APP_PROJECT_NAME} ${CORE_FOUNDATION})
endif ()

set(CMAKE_AUTOMOC OFF)

# MacOSX Bundle ----------------------------------------------------------------

if (APPLE)

	set(MACOSX_BUNDLE_NAME ${PROJECT_NAME})
	set(MACOSX_BINARY_NAME ${APP_PROJECT_NAME})

	set(MACOSX_DYNAMIC_LIBRARIES ${Boost_LIBRARIES} "${LLVM_LIBRARY_DIRS}/libLTO.dylib")
	string(REPLACE ";" " " MACOSX_DYNAMIC_LIBRARIES "${MACOSX_DYNAMIC_LIBRARIES}")

	get_property(QT_CORE_PATH TARGET ${Qt5Core_LIBRARIES} PROPERTY LOCATION)
	get_filename_component(QT_CORE_PATH ${QT_CORE_PATH} REALPATH)

	get_property(QT_GUI_PATH TARGET ${Qt5Gui_LIBRARIES} PROPERTY LOCATION)
	get_filename_component(QT_GUI_PATH ${QT_GUI_PATH} REALPATH)

	get_property(QT_WIDGETS_PATH TARGET ${Qt5Widgets_LIBRARIES} PROPERTY LOCATION)
	get_filename_component(QT_WIDGETS_PATH ${QT_WIDGETS_PATH} REALPATH)

	get_property(QT_PRINT_PATH TARGET ${Qt5PrintSupport_LIBRARIES} PROPERTY LOCATION)
	get_filename_component(QT_PRINT_PATH ${QT_PRINT_PATH} REALPATH)

	list(APPEND MACOSX_QT_FRAMEWORKS ${QT_CORE_PATH} ${QT_GUI_PATH} ${QT_WIDGETS_PATH} ${QT_PRINT_PATH})
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

# CxxTest ----------------------------------------------------------------------

if (UNIX)
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/test/${CMAKE_BUILD_TYPE})
else ()
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/test/)
endif ()

add_subdirectory(src/test)

set(CXXTEST_INCLUDE_DIR $ENV{CXX_TEST_DIR})
set(CXXTEST_PYTHON_TESTGEN_EXECUTABLE ${CXXTEST_INCLUDE_DIR}/bin/cxxtestgen)
set(TESTGEN_FILE ${CMAKE_CURRENT_BINARY_DIR}/test_main.cpp)

find_package(PythonInterp REQUIRED)

add_executable (${TEST_PROJECT_NAME} ${TESTGEN_FILE} ${TEST_FILES})

create_source_groups(${TEST_FILES})

target_link_libraries(${TEST_PROJECT_NAME} ${LIB_PROJECT_NAME})

find_package(CxxTest)
if (CXXTEST_FOUND)
	set_property(
		TARGET ${TEST_PROJECT_NAME}
		PROPERTY INCLUDE_DIRECTORIES
			"${CMAKE_SOURCE_DIR}/src/lib"
			"${CXXTEST_INCLUDE_DIR}"
			"${CMAKE_SOURCE_DIR}/src/external"
	)
endif (CXXTEST_FOUND)

if (WIN32)

	add_custom_command(
		TARGET ${TEST_PROJECT_NAME}
		PRE_BUILD
		COMMAND ${PYTHON_EXECUTABLE} ${CXXTEST_PYTHON_TESTGEN_EXECUTABLE} --runner=ParenPrinter -o ${TESTGEN_FILE} ${PROJECT_SOURCE_DIR}/src/test/*.h
		COMMENT "Generating unittest code with cxxtestgen"
	)

	add_custom_command(
		TARGET ${TEST_PROJECT_NAME}
		POST_BUILD
		COMMAND cd $(ProjectDir)../bin/test/\n$(OutDir)$(TargetName)$(TargetExt)
		COMMENT "Running unittest code"
	)

elseif (UNIX)

	add_custom_command(
		OUTPUT ${TESTGEN_FILE}
		PRE_BUILD
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
