add_files(
	APP_FILES

	data/parser/cxx/TaskParseCxx.cpp

	isTrial.cpp
	main.cpp
)

add_files(
	CLANG_FILES

	data/parser/cxx/name_resolver/CxxDeclNameResolver.cpp
	data/parser/cxx/name_resolver/CxxDeclNameResolver.h
	data/parser/cxx/name_resolver/CxxNameResolver.cpp
	data/parser/cxx/name_resolver/CxxNameResolver.h
	data/parser/cxx/name_resolver/CxxTemplateArgumentNameResolver.cpp
	data/parser/cxx/name_resolver/CxxTemplateArgumentNameResolver.h
	data/parser/cxx/name_resolver/CxxTypeNameResolver.cpp
	data/parser/cxx/name_resolver/CxxTypeNameResolver.h

	data/parser/cxx/ASTAction.cpp
	data/parser/cxx/ASTAction.h
	data/parser/cxx/ASTActionFactory.cpp
	data/parser/cxx/ASTActionFactory.h
	data/parser/cxx/ASTBodyVisitor.cpp
	data/parser/cxx/ASTBodyVisitor.h
	data/parser/cxx/ASTBodyVisitorClient.cpp
	data/parser/cxx/ASTBodyVisitorClient.h
	data/parser/cxx/ASTConsumer.cpp
	data/parser/cxx/ASTConsumer.h
	data/parser/cxx/ASTVisitor.cpp
	data/parser/cxx/ASTVisitor.h
	data/parser/cxx/CommentHandler.cpp
	data/parser/cxx/CommentHandler.h
	data/parser/cxx/CxxDiagnosticConsumer.cpp
	data/parser/cxx/CxxDiagnosticConsumer.h
	data/parser/cxx/CxxParser.cpp
	data/parser/cxx/CxxParser.h
	data/parser/cxx/PreprocessorCallbacks.cpp
	data/parser/cxx/PreprocessorCallbacks.h
	data/parser/cxx/utilityCxx.cpp
	data/parser/cxx/utilityCxx.h
)