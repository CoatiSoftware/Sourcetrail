#include "data/parser/cxx/ASTActionFactory.h"

ASTActionFactory::ASTActionFactory(std::shared_ptr<ParserClient> client)
	: m_client(client)
{
}

ASTActionFactory::~ASTActionFactory()
{
}

clang::FrontendAction* ASTActionFactory::create()
{
	return new ASTAction(m_client);
}
