#include "data/parser/cxx/ASTActionFactory.h"

ASTActionFactory::ASTActionFactory(ParserClient* client, FileRegister* fileRegister)
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
