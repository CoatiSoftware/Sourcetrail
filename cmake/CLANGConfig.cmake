include(cmake/external.cmake)

if (UNIX AND APPLE)
	if ("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
		set(CLANG_BUILD_PATH "$ENV{CLANG_DIR}/build_release")
	else()
		set(CLANG_BUILD_PATH "$ENV{CLANG_DIR}/build_debug")
	endif()
elseif(UNIX)
    set(CLANG_BUILD_PATH "${EXTERNAL_BUILD}/llvm")
else()
	set(CLANG_BUILD_PATH "$ENV{CLANG_DIR}/build")
endif()

execute_process(
	COMMAND ${CLANG_BUILD_PATH}/bin/llvm-config --cxxflags
	OUTPUT_VARIABLE CLANG_DEFINITIONS
)

# Remove unwanted flags
string(REPLACE "-fno-exceptions" "" CLANG_DEFINITIONS "${CLANG_DEFINITIONS}")
string(REPLACE "-fno-rtti" "" CLANG_DEFINITIONS "${CLANG_DEFINITIONS}")

if(UNIX AND NOT APPLE)
    set(CLANG_INCLUDE_DIRS
        "${EXTERNAL_SRC}/clang/include"
        "${CLANG_BUILD_PATH}/tools/clang/include"
        )
else()
    set(CLANG_INCLUDE_DIRS
        "$ENV{CLANG_DIR}/llvm/tools/clang/include"
        "${CLANG_BUILD_PATH}/tools/clang/include"
    )
endif()

set(CLANG_LIBRARY_DIRS "${CLANG_BUILD_PATH}/lib")

set(CLANG_LIBRARIES
	#clangApplyReplacements
	#clangARCMigrate
	#clangCodeGen
	#clangDynamicASTMatchers
	#clangFormat
	#clangFrontendTool
	#clangIndex
	#clangQuer
	#clangTidy
	#clangTidyGoogleModule
	#clangTidyLLVMModule
	#clangTidyMiscModule
	clangASTMatchers
	clangFrontend
	clangSerialization
	clangDriver
	clangTooling
	clangParse
	clangSema
	clangStaticAnalyzerFrontend
	clangStaticAnalyzerCheckers
	clangStaticAnalyzerCore
	clangAnalysis
	clangRewriteFrontend
	clangEdit
	clangAST
	clangLex
	clangBasic
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(CLANG DEFAULT_MSG CLANG_LIBRARIES CLANG_INCLUDE_DIRS)

mark_as_advanced(CLANG_INCLUDE_DIRS CLANG_LIBRARIES)
