# simple check for a git repo
if(EXISTS "${CMAKE_SOURCE_DIR}/.git")
	find_package(Git)

    execute_process(
        COMMAND ${GIT_EXECUTABLE} rev-parse --abbrev-ref HEAD
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE GIT_BRANCH
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )

    execute_process(
        COMMAND ${GIT_EXECUTABLE} log -1 --format=%h
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE GIT_COMMIT_HASH
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )

    execute_process(
        COMMAND ${GIT_EXECUTABLE} log -1 --format=%ci
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE GIT_COMMIT_TIME
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )

    execute_process(
        COMMAND ${GIT_EXECUTABLE} describe --long --match "[0-9]*" HEAD
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE GIT_VERSION_NUMBER
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    string(REGEX REPLACE "^([0-9]+)\\..*" "\\1" VERSION_YEAR "${GIT_VERSION_NUMBER}")
    string(REGEX REPLACE "^[0-9]+\\.([0-9]+).*" "\\1" VERSION_MINOR "${GIT_VERSION_NUMBER}")
    string(REGEX REPLACE "^[0-9]+\\.[0-9]+-([0-9]+).*" "\\1" VERSION_COMMIT "${GIT_VERSION_NUMBER}")

else(EXISTS "${CMAKE_SOURCE_DIR}/.git")
    set(GIT_BRANCH "")
    set(GIT_COMMIT_HASH "")
    set(GIT_VERSION_NUMBER "")
    set(VERSION_YEAR "0")
    set(VERSION_MINOR "0")
    set(VERSION_COMMIT "0")
    set(BUILD_TYPE "")

endif(EXISTS "${CMAKE_SOURCE_DIR}/.git")

set(VERSION_STRING "${VERSION_YEAR}.${VERSION_MINOR}.${VERSION_COMMIT}")

message(STATUS "Version: ${VERSION_STRING}")

# message(STATUS "Git current branch: ${GIT_BRANCH}")
# message(STATUS "Git version number: " ${GIT_VERSION_NUMBER} )
# message(STATUS "Git commit hash: ${GIT_COMMIT_HASH}")
# message(STATUS "Git commit time: ${GIT_COMMIT_TIME}")
# message(STATUS "Version year: ${VERSION_YEAR}")
# message(STATUS "Version minor: ${VERSION_MINOR}")
# message(STATUS "Version commit: ${VERSION_COMMIT}")
