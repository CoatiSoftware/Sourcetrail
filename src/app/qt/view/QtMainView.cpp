#include "qt/view/QtMainView.h"

#include "utility/logging/logging.h"

#include "qt/window/QtMainWindow.h"

QtMainView::QtMainView()
	: m_setTitleFunctor(std::bind(&QtMainView::doSetTitle, this, std::placeholders::_1))
{
	m_window = std::make_shared<QtMainWindow>();
	m_window->show();
	m_window->init();
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
		return;
	}

	m_window->removeView(view);
	m_views.erase(it);
}

void QtMainView::showView(View* view)
{
	m_window->showView(view);
}

void QtMainView::hideView(View* view)
{
	m_window->hideView(view);
}

void QtMainView::loadLayout()
{
	m_window->loadLayout();
}

void QtMainView::saveLayout()
{
	m_window->saveLayout();
}

QStatusBar* QtMainView::getStatusBar()
{
	return m_window->statusBar();
}

void QtMainView::setStatusBar(QStatusBar* statusbar)
{
	m_window->setStatusBar(statusbar);
}

void QtMainView::showStartScreen()
{
	m_window->showStartScreen();
}

void QtMainView::setTitle(const std::string& title)
{
	m_setTitleFunctor(title);
}

void QtMainView::doSetTitle(const std::string& title)
{
	m_window->setWindowTitle(QString::fromStdString(title));
}
