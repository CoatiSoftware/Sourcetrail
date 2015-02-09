#include "data/parser/cxx/ASTActionFactory.h"

ASTActionFactory::ASTActionFactory(ParserClient* client, FileManager* fileManager)
	: m_client(client)
	, m_fileManager(fileManager)
{
}

ASTActionFactory::~ASTActionFactory()
{
}

clang::FrontendAction* ASTActionFactory::create()
{
	return new ASTAction(m_client, m_fileManager);
}
