#include "qt/view/QtMainView.h"

#include "qt/element/QtMainWindow.h"
#include "utility/logging/logging.h"

QtMainView::QtMainView()
{
	m_window = std::make_shared<QtMainWindow>();
	m_window->show();
}

QtMainView::~QtMainView()
{
}

void QtMainView::addView(View* view)
{
	m_views.push_back(view);
	m_window->addView(view);
}

void QtMainView::removeView(View* view)
{
	std::vector<View*>::iterator it = std::find(m_views.begin(), m_views.end(), view);
	if (it == m_views.end())
	{
		LOG_ERROR("View was not found.");
		return;
	}

	m_window->removeView(view);
	m_views.erase(it);
}

void QtMainView::loadLayout()
{
	m_window->loadLayout();
}

void QtMainView::saveLayout()
{
	m_window->saveLayout();
}
