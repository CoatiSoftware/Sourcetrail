#include "Tab.h"

Tab::Tab(
	Id tabId,
	const ViewFactory* viewFactory,
	StorageAccess* storageAccess,
	ScreenSearchSender* screenSearchSender)
	: m_tabId(tabId)
	, m_componentManager(viewFactory, storageAccess)
	, m_parentLayout(nullptr)
	, m_screenSearchSender(screenSearchSender)
{
	m_componentManager.setupTab(this, m_tabId, m_screenSearchSender);
	m_componentManager.refreshViews();
}

Tab::~Tab()
{
	m_componentManager.teardownTab(m_screenSearchSender);
}

void Tab::setParentLayout(ViewLayout* parentLayout)
{
	m_parentLayout = parentLayout;

	if (parentLayout)
	{
		for (View* view: m_views)
		{
			parentLayout->overrideView(view);
		}
	}
}

void Tab::addView(View* view)
{
	m_views.push_back(view);
}

void Tab::removeView(View* view)
{
	std::vector<View*>::iterator it = std::find(m_views.begin(), m_views.end(), view);
	if (it == m_views.end())
	{
		return;
	}

	m_views.erase(it);
}

void Tab::showView(View* view)
{
	if (m_parentLayout)
	{
		m_parentLayout->showView(view);
	}
}

void Tab::hideView(View* view)
{
	if (m_parentLayout)
	{
		m_parentLayout->hideView(view);
	}
}

void Tab::setViewEnabled(View* view, bool enabled)
{
	if (m_parentLayout)
	{
		m_parentLayout->setViewEnabled(view, enabled);
	}
}

void Tab::handleMessage(MessageRefreshUI* message)
{
	m_componentManager.refreshViews();
}
