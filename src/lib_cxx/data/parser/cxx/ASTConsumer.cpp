#include "ASTConsumer.h"

#include "CxxAstVisitor.h"
#include "CxxVerboseAstVisitor.h"
#include "ApplicationSettings.h"

ASTConsumer::ASTConsumer(
	clang::ASTContext* context,
	clang::Preprocessor* preprocessor,
	std::shared_ptr<ParserClient> client,
	std::shared_ptr<CanonicalFilePathCache> canonicalFilePathCache,
	std::shared_ptr<IndexerStateInfo> indexerStateInfo
)
{
	ApplicationSettings* appSettings = ApplicationSettings::getInstance().get();

	if (appSettings->getLoggingEnabled() && appSettings->getVerboseIndexerLoggingEnabled())
	{
		m_visitor = std::make_shared<CxxVerboseAstVisitor>(context, preprocessor, client, canonicalFilePathCache, indexerStateInfo);
	}
	else
	{
		m_visitor = std::make_shared<CxxAstVisitor>(context, preprocessor, client, canonicalFilePathCache, indexerStateInfo);
	}
}

ASTConsumer::~ASTConsumer()
{
}

void ASTConsumer::HandleTranslationUnit(clang::ASTContext& context)
{
	m_visitor->indexDecl(context.getTranslationUnitDecl());
}
