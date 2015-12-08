cmake_minimum_required(VERSION 2.8.9)

# to get CMP0043 warnings, upgrade to version 3 and change the line above to VERSION 3.0 and remove the line below
if(POLICY CMP0043)
   cmake_policy(SET CMP0043 OLD)
endif()

include(cmake/add_files.cmake)
include(cmake/create_source_groups.cmake)
include(cmake/version.cmake)
include(cmake/version_setup.cmake)
include(cmake/licenses.cmake)

# Variables --------------------------------------------------------------------

set(PROJECT_NAME Coati)

set(APP_PROJECT_NAME "${PROJECT_NAME}")
set(TRIAL_PROJECT_NAME "${PROJECT_NAME}_trial")
set(LIB_GUI_PROJECT_NAME "${PROJECT_NAME}_lib_gui")
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
	add_definitions(-std=c++11 -Wno-unknown-warning-option -fcolor-diagnostics -DQT_COMPILING_QSTRING_COMPAT_CPP )
endif ()

# Project ----------------------------------------------------------------------
project(${PROJECT_NAME})

# Clang ------------------------------------------------------------------------

if (UNIX)
	if ("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
		find_package(LLVM REQUIRED PATHS "$ENV{CLANG_DIR}/build_release/share/llvm/cmake")
	else()
		find_package(LLVM REQUIRED PATHS "$ENV{CLANG_DIR}/build_debug/share/llvm/cmake")
	endif()
else()
	find_package(LLVM REQUIRED PATHS "$ENV{CLANG_DIR}/build/share/llvm/cmake")
endif()

find_package(CLANG REQUIRED PATHS "${CMAKE_SOURCE_DIR}/cmake")

add_definitions(${CLANG_DEFINITIONS})

# Boost ------------------------------------------------------------------------

set(BOOST_ROOT $ENV{BOOST_159_DIR})
set(Boost_USE_MULTITHREAD ON)
set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_STATIC_RUNTIME OFF)
Set(Boost_NO_SYSTEM_PATHS ON)
set(BOOST_LIBRARYDIR $ENV{BOOST_159_DIR})

find_package(Boost 1.59 COMPONENTS system filesystem date_time REQUIRED)

# Eigen -------------------------------------------------------------------------

add_definitions(-DEIGEN_MPL2_ONLY)
set(EIGEN_ROOT $ENV{EIGEN_DIR})

# Lib --------------------------------------------------------------------------

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/lib)

add_subdirectory(src/lib)
add_subdirectory(src/external)

set_source_files_properties(${EXTERNAL_FILES} PROPERTIES COMPILE_FLAGS "-w")
set_source_files_properties(${EXTERNAL_C_FILES} PROPERTIES COMPILE_FLAGS "-std=gnu89 -w")

add_library(${LIB_PROJECT_NAME} ${LIB_FILES} ${EXTERNAL_FILES} ${EXTERNAL_C_FILES})

create_source_groups(${LIB_FILES})
create_source_groups(${EXTERNAL_FILES})
create_source_groups(${EXTERNAL_C_FILES})

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
)

target_include_directories(${LIB_PROJECT_NAME} SYSTEM
	PUBLIC ${LLVM_INCLUDE_DIRS}
	${CLANG_INCLUDE_DIRS}
	${Boost_INCLUDE_DIRS}
	${EIGEN_ROOT}
    "${CMAKE_SOURCE_DIR}/src/external"
)


link_directories(${LLVM_LIBRARY_DIRS} ${CLANG_LIBRARY_DIRS} ${Boost_LIBRARY_DIRS})

target_link_libraries(${LIB_PROJECT_NAME} ${CLANG_LIBRARIES} ${LLVM_AVAILABLE_LIBS} ${Boost_LIBRARIES})

# Lib Gui --------------------------------------------------------------------------

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/lib_gui)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/lib_gui)

add_subdirectory(src/lib_gui)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

# Find the QtWidgets library
set(CMAKE_PREFIX_PATH "$ENV{QT_DIR}/")

set(Qt5Widgets_DIR "$ENV{QT_DIR}/cmake/Qt5Widgets/")
set(Qt5PrintSupport_DIR "$ENV{QT_DIR}/cmake/Qt5PrintSupport/")

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

add_library(${LIB_GUI_PROJECT_NAME} ${LIB_GUI_FILES} ${CMAKE_SOURCE_DIR}/build/src/lib_gui/version.h)

# command for versioning script
add_custom_command(
	TARGET versionnumber
	PRE_BUILD
	COMMAND ${CMAKE_COMMAND} -P ${CMAKE_SOURCE_DIR}/cmake/version.cmake
	BYPRODUCTS ${PROJECT_SOURCE_DIR}/build/src/lib_gui/version.h
	DEPENDS ${LIB_GUI_PROJECT_NAME}
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
	COMMENT "check/update version number"
)
add_dependencies(${LIB_GUI_PROJECT_NAME} versionnumber)

create_source_groups(${LIB_GUI_FILES})

set_property(
	TARGET ${LIB_GUI_PROJECT_NAME}
	PROPERTY INCLUDE_DIRECTORIES
		"${CMAKE_SOURCE_DIR}/src/lib"
		"${CMAKE_SOURCE_DIR}/src/lib_gui"
		"${CMAKE_SOURCE_DIR}/build/src/lib_gui"
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
	"${PROJECT_SOURCE_DIR}/build/src/lib_gui/includes.h"
)

#configure the versioning file
configure_file(
  ${CMAKE_SOURCE_DIR}/src/lib_gui/version.h.in
  ${CMAKE_SOURCE_DIR}/build/src/lib_gui/version.h
)

set(CMAKE_AUTOMOC OFF)

# App --------------------------------------------------------------------------

if (UNIX)
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/app/${CMAKE_BUILD_TYPE})
else ()
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/app/)
endif ()

add_subdirectory(src/app)

set_source_files_properties(${CLANG_FILES} PROPERTIES COMPILE_FLAGS "-fno-rtti")

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

if (WIN32)
	file(WRITE ${CMAKE_SOURCE_DIR}/build/Coati.rc
		"// Icon with lowest ID value placed first to ensure application icon\n"
		"// remains consistent on all systems.\n"
		"IDI_ICON1               ICON                    \"${CMAKE_SOURCE_DIR}/build/coati.ico\"\n"
	)
	add_executable(${APP_PROJECT_NAME} ${APP_FILES} ${CLANG_FILES} ${CMAKE_SOURCE_DIR}/build/Coati.rc)

	# hide the console when running a release build.
	set_target_properties(${APP_PROJECT_NAME} PROPERTIES LINK_FLAGS_DEBUG "/SUBSYSTEM:CONSOLE")
	set_target_properties(${APP_PROJECT_NAME} PROPERTIES COMPILE_DEFINITIONS_DEBUG "_CONSOLE")
	set_target_properties(${APP_PROJECT_NAME} PROPERTIES LINK_FLAGS_RELWITHDEBINFO "/SUBSYSTEM:CONSOLE")
	set_target_properties(${APP_PROJECT_NAME} PROPERTIES COMPILE_DEFINITIONS_RELWITHDEBINFO "_CONSOLE")
	set_target_properties(${APP_PROJECT_NAME} PROPERTIES LINK_FLAGS_RELEASE "/ENTRY:\"mainCRTStartup\" /SUBSYSTEM:WINDOWS")
	set_target_properties(${APP_PROJECT_NAME} PROPERTIES LINK_FLAGS_MINSIZEREL "/SUBSYSTEM:WINDOWS")

else ()
	add_executable(${APP_PROJECT_NAME} ${APP_FILES} ${CLANG_FILES})
endif ()

create_source_groups(${APP_FILES})
create_source_groups(${CLANG_FILES})

target_link_libraries(${APP_PROJECT_NAME} ${LIB_GUI_PROJECT_NAME} ${LIB_PROJECT_NAME})

set_property(
	TARGET ${APP_PROJECT_NAME}
	PROPERTY INCLUDE_DIRECTORIES
		"${CMAKE_SOURCE_DIR}/src/app"
		"${CMAKE_SOURCE_DIR}/src/lib"
		"${CMAKE_SOURCE_DIR}/src/lib_gui"
		"${CMAKE_SOURCE_DIR}/build/src/lib_gui"
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

# Trial ---------------------------------------------------------------------

if (UNIX)
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/app/${CMAKE_BUILD_TYPE})
else ()
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/app/)
endif ()

add_subdirectory(src/trial)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

if (WIN32)
	file(WRITE ${CMAKE_SOURCE_DIR}/build/Coati.rc
		"// Icon with lowest ID value placed first to ensure application icon\n"
		"// remains consistent on all systems.\n"
		"IDI_ICON1               ICON                    \"${CMAKE_SOURCE_DIR}/build/coati.ico\"\n"
	)
	add_executable(${TRIAL_PROJECT_NAME} ${TRIAL_FILES} ${CMAKE_SOURCE_DIR}/build/Coati.rc)

	# hide the console when running a release build.
	set_target_properties(${TRIAL_PROJECT_NAME} PROPERTIES LINK_FLAGS_DEBUG "/SUBSYSTEM:CONSOLE")
	set_target_properties(${TRIAL_PROJECT_NAME} PROPERTIES COMPILE_DEFINITIONS_DEBUG "_CONSOLE")
	set_target_properties(${TRIAL_PROJECT_NAME} PROPERTIES LINK_FLAGS_RELWITHDEBINFO "/SUBSYSTEM:CONSOLE")
	set_target_properties(${TRIAL_PROJECT_NAME} PROPERTIES COMPILE_DEFINITIONS_RELWITHDEBINFO "_CONSOLE")
	set_target_properties(${TRIAL_PROJECT_NAME} PROPERTIES LINK_FLAGS_RELEASE "/ENTRY:\"mainCRTStartup\" /SUBSYSTEM:WINDOWS")
	set_target_properties(${TRIAL_PROJECT_NAME} PROPERTIES LINK_FLAGS_MINSIZEREL "/SUBSYSTEM:WINDOWS")

else ()
	add_executable(${TRIAL_PROJECT_NAME} ${TRIAL_FILES})
endif ()

create_source_groups(${TRIAL_FILES})

target_link_libraries(${TRIAL_PROJECT_NAME} ${LIB_GUI_PROJECT_NAME} ${LIB_PROJECT_NAME})

set_property(
	TARGET ${TRIAL_PROJECT_NAME}
	PROPERTY INCLUDE_DIRECTORIES
		"${CMAKE_SOURCE_DIR}/src/trial"
		"${CMAKE_SOURCE_DIR}/src/lib"
		"${CMAKE_SOURCE_DIR}/src/lib_gui"
		"${CMAKE_SOURCE_DIR}/build/src/lib_gui"
)

target_include_directories(${TRIAL_PROJECT_NAME} SYSTEM
		PUBLIC "${CMAKE_SOURCE_DIR}/src/external"
		PUBLIC ${Boost_INCLUDE_DIRS}
)

# Use the Widgets module from Qt 5.
qt5_use_modules(${TRIAL_PROJECT_NAME} Widgets)
qt5_use_modules(${TRIAL_PROJECT_NAME} Network)

# add platform specific libraries
if (APPLE)
	find_library(CORE_FOUNDATION CoreFoundation)
	target_link_libraries(${TRIAL_PROJECT_NAME} ${CORE_FOUNDATION})
endif ()

# MacOSX Bundle ----------------------------------------------------------------

if (APPLE)

	set(MACOSX_BUNDLE_NAME ${PROJECT_NAME})
	set(MACOSX_BINARY_NAME ${APP_PROJECT_NAME})

	set(MACOSX_DYNAMIC_LIBRARIES ${Boost_LIBRARIES} "${LLVM_LIBRARY_DIRS}/libLTO.3.8.dylib")
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
			${Boost_INCLUDE_DIRS}
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

# Linux package
if(UNIX AND NOT APPLE)
	include(cmake/LinuxPackage.cmake)
endif  ()
