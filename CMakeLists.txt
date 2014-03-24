cmake_minimum_required(VERSION 2.8.9)

include(cmake/add_files.cmake)


# Variables --------------------------------------------------------------------

set(PROJECT_NAME masterproject)

set(APP_PROJECT_NAME "${PROJECT_NAME}_app")
set(LIB_PROJECT_NAME "${PROJECT_NAME}_lib")
set(TEST_PROJECT_NAME "${PROJECT_NAME}_test")


# Settings ---------------------------------------------------------------------

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)

if (UNIX)
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE})
else ()
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
endif ()


# Project ----------------------------------------------------------------------

project(${PROJECT_NAME})


# Qt ---------------------------------------------------------------------------

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

# Find the QtWidgets library
set(CMAKE_PREFIX_PATH "$ENV{QT_DIR}/")

find_package(Qt5Widgets)


# Lib --------------------------------------------------------------------------

add_subdirectory(src/lib)

add_library(${LIB_PROJECT_NAME} ${LIB_FILES})

set_target_properties(	${LIB_PROJECT_NAME} 
						PROPERTIES 	DEBUG_OUTPUT_NAME ${PROJECT_NAME}_d
									RELEASE_OUTPUT_NAME ${PROJECT_NAME})


# App --------------------------------------------------------------------------


add_subdirectory(src/app)
include_directories(src/lib)

add_executable(${APP_PROJECT_NAME} ${APP_FILES})

target_link_libraries(${APP_PROJECT_NAME} ${LIB_PROJECT_NAME})

# Use the Widgets module from Qt 5.
qt5_use_modules(${APP_PROJECT_NAME} Widgets)


# CxxTest ----------------------------------------------------------------------

add_subdirectory(src/test)

set(CXXTEST_INCLUDE_DIR $ENV{CXX_TEST_DIR})
set(CXXTEST_PYTHON_TESTGEN_EXECUTABLE ${CXXTEST_INCLUDE_DIR}/bin/cxxtestgen)
set(TESTGEN_FILE ${CMAKE_CURRENT_BINARY_DIR}/temptests.cpp)

find_package(CxxTest)

if (CXXTEST_FOUND)

    include_directories(${CXXTEST_INCLUDE_DIR})
	
endif (CXXTEST_FOUND)

find_package(PythonInterp REQUIRED)

add_executable (${TEST_PROJECT_NAME} ${TESTGEN_FILE})
									
target_link_libraries(${TEST_PROJECT_NAME} ${LIB_PROJECT_NAME})

if (WIN32)

	add_custom_command(TARGET ${TEST_PROJECT_NAME}
					   PRE_BUILD
					   COMMAND ${PYTHON_EXECUTABLE} ${CXXTEST_PYTHON_TESTGEN_EXECUTABLE} --runner=ParenPrinter -o ${TESTGEN_FILE} ${PROJECT_SOURCE_DIR}/src/test/*.h
					   COMMENT "Generating unittest code with cxxtestgen")
					   
	add_custom_command(TARGET ${TEST_PROJECT_NAME}
					   POST_BUILD
					   COMMAND $(OutDir)$(TargetName)$(TargetExt)
					   COMMENT "Running unittest code")
					   
elseif (UNIX)

	add_custom_command(OUTPUT ${TESTGEN_FILE}
					   PRE_BUILD
					   COMMAND ${PYTHON_EXECUTABLE} ${CXXTEST_PYTHON_TESTGEN_EXECUTABLE} --runner=ParenPrinter -o ${TESTGEN_FILE} ${PROJECT_SOURCE_DIR}/src/test/*.h
					   COMMENT "Generating unittest code with cxxtestgen")

	add_custom_target(gen DEPENDS ${TESTGEN_FILE})

	add_dependencies(${TEST_PROJECT_NAME} gen)

endif ()
