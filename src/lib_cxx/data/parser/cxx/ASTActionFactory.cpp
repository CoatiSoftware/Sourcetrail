#include "data/parser/cxx/ASTActionFactory.h"

#include "clang/Frontend/FrontendActions.h"
#include "data/parser/cxx/ASTAction.h"

ASTActionFactory::ASTActionFactory(
	std::shared_ptr<ParserClient> client,
	std::shared_ptr<FileRegister> fileRegister,
	std::shared_ptr<FilePathCache> canonicalFilePathCache,
	bool preprocessorOnly
)
	: m_client(client)
	, m_fileRegister(fileRegister)
	, m_canonicalFilePathCache(canonicalFilePathCache)
	, m_preprocessorOnly(preprocessorOnly)
{
}

ASTActionFactory::~ASTActionFactory()
{
}

clang::FrontendAction* ASTActionFactory::create()
{
	if (m_preprocessorOnly)
	{
		return new ASTAction<clang::PreprocessOnlyAction>(m_client, m_fileRegister, m_canonicalFilePathCache);
	}
	else
	{
		return new ASTAction<clang::ASTFrontendAction>(m_client, m_fileRegister, m_canonicalFilePathCache);
	}
}
