#include "data/parser/cxx/ASTConsumer.h"

#include "data/parser/ParserClient.h"

ASTConsumer::ASTConsumer(clang::ASTContext* context, ParserClient* client, FileRegister* fileRegister)
	: m_visitor(context, client, fileRegister)
{
}

ASTConsumer::~ASTConsumer()
{
}

void ASTConsumer::HandleTranslationUnit(clang::ASTContext& context)
{
	m_visitor.TraverseDecl(context.getTranslationUnitDecl());
}
