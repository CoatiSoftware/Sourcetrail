#include "component/view/TabbedView.h"

#include <algorithm>

TabbedView::TabbedView(ViewLayout* viewLayout, const std::string& name)
	: View(viewLayout)
	, m_name(name)
{
}

TabbedView::~TabbedView()
{
}

const std::vector<View*>& TabbedView::getViews() const
{
	return m_views;
}

std::string TabbedView::getName() const
{
	return m_name;
}

void TabbedView::addView(View* view)
{
	m_views.push_back(view);

	addViewWidget(view);
}

void TabbedView::removeView(View* view)
{
	std::vector<View*>::iterator it = std::find(m_views.begin(), m_views.end(), view);
	if (it == m_views.end())
	{
		return;
	}

	m_views.erase(it);
}

void TabbedView::showView(View* view)
{
	getViewLayout()->showView(view);
}

void TabbedView::hideView(View* view)
{
	getViewLayout()->hideView(view);
}
