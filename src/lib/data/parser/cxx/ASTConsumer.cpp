#include "data/parser/cxx/ASTConsumer.h"

#include "data/parser/ParserClient.h"

ASTConsumer::ASTConsumer(clang::ASTContext* context, ParserClient* client, FileManager* fileManager)
	: m_visitor(context, client, fileManager)
{
}

ASTConsumer::~ASTConsumer()
{
}

void ASTConsumer::HandleTranslationUnit(clang::ASTContext& context)
{
	m_visitor.TraverseDecl(context.getTranslationUnitDecl());
}
