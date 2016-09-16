#include "data/parser/cxx/ASTConsumer.h"

#include "data/parser/ParserClient.h"

ASTConsumer::ASTConsumer(clang::ASTContext* context, clang::Preprocessor* preprocessor, ParserClient* client, FileRegister* fileRegister)
	: m_visitor(context, preprocessor, client, fileRegister)
{
}

ASTConsumer::~ASTConsumer()
{
}

void ASTConsumer::HandleTranslationUnit(clang::ASTContext& context)
{
	m_visitor.indexDecl(context.getTranslationUnitDecl());
}
