#include "ASTActionFactory.h"

#include <clang/Frontend/FrontendActions.h>

#include "ASTAction.h"

ASTActionFactory::ASTActionFactory(
	std::shared_ptr<ParserClient> client,
	std::shared_ptr<CanonicalFilePathCache> canonicalFilePathCache
)
	: m_client(client)
	, m_canonicalFilePathCache(canonicalFilePathCache)
{
}

ASTActionFactory::~ASTActionFactory()
{
}

clang::FrontendAction* ASTActionFactory::create()
{
	return new ASTAction<clang::ASTFrontendAction>(m_client, m_canonicalFilePathCache);
}
