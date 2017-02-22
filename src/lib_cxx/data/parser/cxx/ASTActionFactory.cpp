#include "data/parser/cxx/ASTActionFactory.h"

ASTActionFactory::ASTActionFactory(std::shared_ptr<ParserClient> client, std::shared_ptr<FileRegister> fileRegister)
	: m_client(client)
	, m_fileRegister(fileRegister)
{
}

ASTActionFactory::~ASTActionFactory()
{
}

clang::FrontendAction* ASTActionFactory::create()
{
	return new ASTAction(m_client, m_fileRegister);
}
