#include "data/parser/cxx/ASTConsumer.h"

#include "data/parser/cxx/CxxAstVisitor.h"
#include "data/parser/cxx/CxxVerboseAstVisitor.h"
#include "settings/ApplicationSettings.h"

ASTConsumer::ASTConsumer(
	clang::ASTContext* context,
	clang::Preprocessor* preprocessor,
	std::shared_ptr<ParserClient> client,
	std::shared_ptr<CanonicalFilePathCache> canonicalFilePathCache
)
{
	ApplicationSettings* appSettings = ApplicationSettings::getInstance().get();

	if (appSettings->getLoggingEnabled() && appSettings->getVerboseIndexerLoggingEnabled())
	{
		m_visitor = std::make_shared<CxxVerboseAstVisitor>(context, preprocessor, client, canonicalFilePathCache);
	}
	else
	{
		m_visitor = std::make_shared<CxxAstVisitor>(context, preprocessor, client, canonicalFilePathCache);
	}
}

ASTConsumer::~ASTConsumer()
{
}

void ASTConsumer::HandleTranslationUnit(clang::ASTContext& context)
{
	m_visitor->indexDecl(context.getTranslationUnitDecl());
}
