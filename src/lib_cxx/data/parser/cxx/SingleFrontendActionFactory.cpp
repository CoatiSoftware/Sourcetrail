#include "SingleFrontendActionFactory.h"

SingleFrontendActionFactory::SingleFrontendActionFactory(clang::FrontendAction* action)
	: m_action(action)
{
}

clang::FrontendAction* SingleFrontendActionFactory::create()
{
	return m_action;
}
