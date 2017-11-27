#include "data/parser/cxx/ASTActionFactory.h"

#include "clang/Frontend/FrontendActions.h"
#include "data/parser/cxx/ASTAction.h"

ASTActionFactory::ASTActionFactory(
	std::shared_ptr<ParserClient> client,
	std::shared_ptr<FileRegister> fileRegister,
	std::shared_ptr<CanonicalFilePathCache> canonicalFilePathCache
)
	: m_client(client)
	, m_fileRegister(fileRegister)
	, m_canonicalFilePathCache(canonicalFilePathCache)
{
}

ASTActionFactory::~ASTActionFactory()
{
}

clang::FrontendAction* ASTActionFactory::create()
{
	return new ASTAction<clang::ASTFrontendAction>(m_client, m_fileRegister, m_canonicalFilePathCache);
}
