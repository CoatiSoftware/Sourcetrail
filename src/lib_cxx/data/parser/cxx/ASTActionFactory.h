#ifndef AST_ACTION_FACTORY
#define AST_ACTION_FACTORY

#include "clang/Tooling/Tooling.h"

#include "data/parser/cxx/ASTAction.h"
#include "data/parser/cxx/cxxCacheTypes.h"
#include "utility/file/FileRegister.h"

class ASTActionFactory
	: public clang::tooling::FrontendActionFactory
{
public:
	explicit ASTActionFactory(
		std::shared_ptr<ParserClient> client,
		std::shared_ptr<FileRegister> fileRegister,
		std::shared_ptr<FilePathCache> canonicalFilePathCache,
		bool preprocessorOnly
	);

	virtual ~ASTActionFactory();

	virtual clang::FrontendAction* create();

private:
	std::shared_ptr<ParserClient> m_client;
	std::shared_ptr<FileRegister> m_fileRegister;
	std::shared_ptr<FilePathCache> m_canonicalFilePathCache;

	bool m_preprocessorOnly;
};

#endif // AST_ACTION_FACTORY
