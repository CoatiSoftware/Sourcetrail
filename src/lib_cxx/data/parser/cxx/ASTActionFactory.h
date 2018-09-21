#ifndef AST_ACTION_FACTORY
#define AST_ACTION_FACTORY

#include <memory>

#include <clang/Tooling/Tooling.h>

#include "IndexerStateInfo.h"

class CanonicalFilePathCache;
class ParserClient;

class ASTActionFactory
	: public clang::tooling::FrontendActionFactory
{
public:
	explicit ASTActionFactory(
		std::shared_ptr<ParserClient> client,
		std::shared_ptr<CanonicalFilePathCache> canonicalFilePathCache,
		std::shared_ptr<IndexerStateInfo> indexerStateInfo
	);

	virtual ~ASTActionFactory();

	virtual clang::FrontendAction* create() override;

private:
	std::shared_ptr<ParserClient> m_client;
	std::shared_ptr<CanonicalFilePathCache> m_canonicalFilePathCache;
	std::shared_ptr<IndexerStateInfo> m_indexerStateInfo;
};

#endif // AST_ACTION_FACTORY
