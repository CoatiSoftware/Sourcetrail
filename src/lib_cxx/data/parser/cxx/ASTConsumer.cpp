#include "data/parser/cxx/ASTConsumer.h"
#include "data/parser/cxx/CxxAstVisitor.h"
#include "data/parser/cxx/CxxVerboseAstVisitor.h"
#include "settings/ApplicationSettings.h"

ASTConsumer::ASTConsumer(clang::ASTContext* context, clang::Preprocessor* preprocessor, ParserClient* client, FileRegister* fileRegister)
{
	if (ApplicationSettings::getInstance()->getVerboseInderxerLoggingEnabled())
	{
		m_visitor = std::make_shared<CxxVerboseAstVisitor>(context, preprocessor, client, fileRegister);
	}
	else
	{
		m_visitor = std::make_shared<CxxAstVisitor>(context, preprocessor, client, fileRegister);
	}
}

ASTConsumer::~ASTConsumer()
{
}

void ASTConsumer::HandleTranslationUnit(clang::ASTContext& context)
{
	m_visitor->indexDecl(context.getTranslationUnitDecl());
}
