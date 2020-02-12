#include "Tab.h"

#include "CodeView.h"
#include "GraphView.h"

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

Id Tab::getSchedulerId() const
{
	return m_tabId;
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

void Tab::handleMessage(MessageFocusView* message)
{
	GraphView* graphView = dynamic_cast<GraphView*>(m_componentManager.getView(GraphView::VIEW_NAME));
	CodeView* codeView = dynamic_cast<CodeView*>(m_componentManager.getView(CodeView::VIEW_NAME));

	if (!graphView || !codeView)
	{
		LOG_ERROR("Tab has no code or graph view.");
		return;
	}

	MessageFocusView::ViewType type = message->type;
	if (type == MessageFocusView::ViewType::TOGGLE)
	{
		if (graphView->hasNavigationFocus())
		{
			type = MessageFocusView::ViewType::CODE;
		}
		else
		{
			type = MessageFocusView::ViewType::GRAPH;
		}
	}

	graphView->setNavigationFocus(type == MessageFocusView::ViewType::GRAPH);
	codeView->setNavigationFocus(type == MessageFocusView::ViewType::CODE);
}

void Tab::handleMessage(MessageRefreshUI* message)
{
	m_componentManager.refreshViews();
}
