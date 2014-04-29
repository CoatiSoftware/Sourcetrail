execute_process(
	COMMAND $ENV{CLANG_DIR}/build/bin/llvm-config --cxxflags
	OUTPUT_VARIABLE CLANG_DEFINITIONS
)

set(CLANG_INCLUDE_DIRS
	"$ENV{CLANG_DIR}/llvm/tools/clang/include"
	"$ENV{CLANG_DIR}/build/tools/clang/include"
)

set(CLANG_LIBRARY_DIRS "$ENV{CLANG_DIR}/build/lib")

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
	clangRewriteCore
	clangEdit
	clangAST
	clangLex
	clangBasic
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(CLANG DEFAULT_MSG CLANG_LIBRARIES CLANG_INCLUDE_DIRS)

mark_as_advanced(CLANG_INCLUDE_DIRS CLANG_LIBRARIES)
