
add_files(
	CLANG_FILES

	data/parser/cxx/name/CxxDeclName.cpp
	data/parser/cxx/name/CxxDeclName.h
	data/parser/cxx/name/CxxFunctionDeclName.cpp
	data/parser/cxx/name/CxxFunctionDeclName.h
	data/parser/cxx/name/CxxName.cpp
	data/parser/cxx/name/CxxName.h
	data/parser/cxx/name/CxxQualifierFlags.cpp
	data/parser/cxx/name/CxxQualifierFlags.h
	data/parser/cxx/name/CxxTypeName.cpp
	data/parser/cxx/name/CxxTypeName.h
	
	data/parser/cxx/name_resolver/CxxDeclNameResolver.cpp
	data/parser/cxx/name_resolver/CxxDeclNameResolver.h
	data/parser/cxx/name_resolver/CxxNameResolver.cpp
	data/parser/cxx/name_resolver/CxxNameResolver.h
	data/parser/cxx/name_resolver/CxxSpecifierNameResolver.cpp
	data/parser/cxx/name_resolver/CxxSpecifierNameResolver.h
	data/parser/cxx/name_resolver/CxxTemplateArgumentNameResolver.cpp
	data/parser/cxx/name_resolver/CxxTemplateArgumentNameResolver.h
	data/parser/cxx/name_resolver/CxxTypeNameResolver.cpp
	data/parser/cxx/name_resolver/CxxTypeNameResolver.h

	data/parser/cxx/ASTAction.cpp
	data/parser/cxx/ASTAction.h
	data/parser/cxx/ASTActionFactory.cpp
	data/parser/cxx/ASTActionFactory.h
	data/parser/cxx/ASTConsumer.cpp
	data/parser/cxx/ASTConsumer.h
	data/parser/cxx/CommentHandler.cpp
	data/parser/cxx/CommentHandler.h
	data/parser/cxx/CxxAstVisitor.cpp
	data/parser/cxx/CxxAstVisitor.h
	data/parser/cxx/CxxCompilationDatabaseSingle.cpp
	data/parser/cxx/CxxCompilationDatabaseSingle.h
	data/parser/cxx/CxxContext.cpp
	data/parser/cxx/CxxContext.h
	data/parser/cxx/CxxDiagnosticConsumer.cpp
	data/parser/cxx/CxxDiagnosticConsumer.h
	data/parser/cxx/CxxParser.cpp
	data/parser/cxx/CxxParser.h
	data/parser/cxx/CxxVerboseAstVisitor.cpp
	data/parser/cxx/CxxVerboseAstVisitor.h
	data/parser/cxx/PreprocessorCallbacks.cpp
	data/parser/cxx/PreprocessorCallbacks.h
	data/parser/cxx/utilityCxx.cpp
	data/parser/cxx/utilityCxx.h

	utility/CompilationDatabase.cpp
	utility/CompilationDatabase.h

)

add_files(
	LIB_CXX_FILES

	data/parser/cxx/TaskParseCxx.cpp
	data/parser/cxx/TaskParseCxx.h

	CxxProject.cpp
	CxxProject.h
	
	ProjectFactoryModuleCpp.cpp
	ProjectFactoryModuleCpp.h
	ProjectFactoryModuleC.cpp
	ProjectFactoryModuleC.h
)
