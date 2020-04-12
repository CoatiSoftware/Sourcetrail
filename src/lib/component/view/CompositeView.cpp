#include "CompositeView.h"

#include <algorithm>

CompositeView::CompositeView(
	ViewLayout* viewLayout, CompositeDirection direction, const std::string& name, Id tabId)
	: View(viewLayout), m_direction(direction), m_name(name), m_tabId(tabId)
{
}

CompositeView::~CompositeView() {}

Id CompositeView::getSchedulerId() const
{
	return m_tabId;
}

CompositeView::CompositeDirection CompositeView::getDirection() const
{
	return m_direction;
}

const std::vector<View*>& CompositeView::getViews() const
{
	return m_views;
}

std::string CompositeView::getName() const
{
	return m_name;
}

void CompositeView::addView(View* view)
{
	m_views.push_back(view);

	addViewWidget(view);
}

void CompositeView::removeView(View* view)
{
	std::vector<View*>::iterator it = std::find(m_views.begin(), m_views.end(), view);
	if (it == m_views.end())
	{
		return;
	}

	m_views.erase(it);
}

void CompositeView::showView(View* view) {}

void CompositeView::hideView(View* view) {}

void CompositeView::setViewEnabled(View* view, bool enabled)
{
	getViewLayout()->setViewEnabled(view, enabled);
}

void CompositeView::handleMessage(MessageFocusedSearchView* message)
{
	showFocusIndicator(message->focusIn);
}
