if(UNIX AND NOT APPLE)

	execute_process(
		COMMAND $ENV{CLANG_DIR}/build_debug/bin/llvm-config --cxxflags
		OUTPUT_VARIABLE CLANG_DEFINITIONS
	)


	set(CLANG_INCLUDE_DIRS
		"$ENV{CLANG_DIR}/llvm/tools/clang/include"
		"$ENV{CLANG_DIR}/build_debug/tools/clang/include"
	)

	set(CLANG_LIBRARY_DIRS "$ENV{CLANG_DIR}/build_debug/lib")
	if (UNIX)
		if ("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
			set(CLANG_LIBRARY_DIRS "$ENV{CLANG_DIR}/build_release/lib")
		endif()
	endif ()

else()

	execute_process(
		COMMAND $ENV{CLANG_DIR}/build/bin/llvm-config --cxxflags
		OUTPUT_VARIABLE CLANG_DEFINITIONS
	)

	set(CLANG_INCLUDE_DIRS
		"$ENV{CLANG_DIR}/llvm/tools/clang/include"
		"$ENV{CLANG_DIR}/build/tools/clang/include"
	)

	set(CLANG_LIBRARY_DIRS "$ENV{CLANG_DIR}/build_debug/lib")
	if (UNIX)
		if ("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
			set(CLANG_LIBRARY_DIRS "$ENV{CLANG_DIR}/build/Release+Asserts/lib")
		endif()
	endif ()

endif()

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
