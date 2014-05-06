#include "data/parser/cxx/ASTConsumer.h"

ASTConsumer::ASTConsumer(clang::ASTContext* context, std::shared_ptr<ParserClient> client)
	: m_visitor(context, client)
{
}

ASTConsumer::~ASTConsumer()
{
}

void ASTConsumer::HandleTranslationUnit(clang::ASTContext& context)
{
	m_visitor.TraverseDecl(context.getTranslationUnitDecl());
}
