#include "data/parser/cxx/ASTConsumer.h"

#include "data/parser/cxx/CxxAstVisitor.h"
#include "data/parser/cxx/CxxVerboseAstVisitor.h"
#include "settings/ApplicationSettings.h"

ASTConsumer::ASTConsumer(
	clang::ASTContext* context,
	clang::Preprocessor* preprocessor,
	std::shared_ptr<ParserClient> client,
	std::shared_ptr<FileRegister> fileRegister,
	std::shared_ptr<FilePathCache> canonicalFilePathCache
)
{
	ApplicationSettings* appSettings = ApplicationSettings::getInstance().get();

	if (appSettings->getLoggingEnabled() && appSettings->getVerboseIndexerLoggingEnabled())
	{
		m_visitor = std::make_shared<CxxVerboseAstVisitor>(context, preprocessor, client, fileRegister, canonicalFilePathCache);
	}
	else
	{
		m_visitor = std::make_shared<CxxAstVisitor>(context, preprocessor, client, fileRegister, canonicalFilePathCache);
	}
}

ASTConsumer::~ASTConsumer()
{
}

void ASTConsumer::HandleTranslationUnit(clang::ASTContext& context)
{
	m_visitor->indexDecl(context.getTranslationUnitDecl());
}
