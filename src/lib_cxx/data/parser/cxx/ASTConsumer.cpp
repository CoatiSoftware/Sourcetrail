#include "data/parser/cxx/ASTConsumer.h"
#include "data/parser/cxx/CxxAstVisitor.h"
#include "data/parser/cxx/CxxVerboseAstVisitor.h"
#include "settings/ApplicationSettings.h"

ASTConsumer::ASTConsumer(clang::ASTContext* context, clang::Preprocessor* preprocessor, std::shared_ptr<ParserClient> client, std::shared_ptr<FileRegister> fileRegister)
{
	if (ApplicationSettings::getInstance()->getLoggingEnabled() && ApplicationSettings::getInstance()->getVerboseIndexerLoggingEnabled())
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
