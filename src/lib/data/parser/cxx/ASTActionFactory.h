#ifndef AST_ACTION_FACTORY
#define AST_ACTION_FACTORY

#include "clang/Tooling/Tooling.h"

#include "data/parser/cxx/ASTAction.h"

class ASTActionFactory : public clang::tooling::FrontendActionFactory
{
public:
	explicit ASTActionFactory(ParserClient* client);
	virtual ~ASTActionFactory();

	virtual clang::FrontendAction* create();

private:
	ParserClient* m_client;
};

#endif // AST_ACTION_FACTORY
