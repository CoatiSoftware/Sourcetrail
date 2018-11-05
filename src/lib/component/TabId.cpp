#include "TabId.h"

Id TabId::s_nextTabId = 10;
Id TabId::s_currentTabId = 0;

Id TabId::app()
{
	return 1;
}

Id TabId::background()
{
	return 2;
}

Id TabId::ignore()
{
	return 3;
}

Id TabId::nextTab()
{
	return s_nextTabId++;
}

Id TabId::currentTab()
{
	return s_currentTabId;
}

void TabId::setCurrentTabId(Id currentTabId)
{
	s_currentTabId = currentTabId;
}
