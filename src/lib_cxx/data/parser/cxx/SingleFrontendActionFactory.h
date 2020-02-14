#ifndef SINGLE_FRONTEND_ACTION_FACTORY
#define SINGLE_FRONTEND_ACTION_FACTORY

#include <clang/Tooling/Tooling.h>

class SingleFrontendActionFactory: public clang::tooling::FrontendActionFactory
{
public:
	SingleFrontendActionFactory(clang::FrontendAction* action);
	clang::FrontendAction* create() override;

private:
	clang::FrontendAction* m_action;
};

#endif	  // SINGLE_FRONTEND_ACTION_FACTORY
