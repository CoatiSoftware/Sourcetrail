#include "data/parser/cxx/ASTActionFactory.h"

#include "clang/Frontend/FrontendActions.h"

ASTActionFactory::ASTActionFactory(
	std::shared_ptr<ParserClient> client, std::shared_ptr<FileRegister> fileRegister, bool preprocessorOnly
)
	: m_client(client)
	, m_fileRegister(fileRegister)
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
		return new ASTAction<clang::PreprocessOnlyAction>(m_client, m_fileRegister);
	}
	else
	{
		return new ASTAction<clang::ASTFrontendAction>(m_client, m_fileRegister);
	}
}
