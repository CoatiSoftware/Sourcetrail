#include "QtStatusBarView.h"

#include <QStatusBar>

#include "QtMainView.h"

QtStatusBarView::QtStatusBarView(ViewLayout* viewLayout)
	: StatusBarView(viewLayout)
{
	m_widget = std::make_shared<QtStatusBar>();
	m_widget->show();

	QtMainView* mw = static_cast<QtMainView*>(viewLayout);
	QStatusBar* sb = static_cast<QStatusBar*>(m_widget.get());
	mw->setStatusBar(sb);
}

void QtStatusBarView::createWidgetWrapper()
{
}

void QtStatusBarView::initView()
{
}

void QtStatusBarView::refreshView()
{
}

void QtStatusBarView::showMessage(const std::wstring& message, bool isError, bool showLoader)
{
	m_onQtThread(
		[=]()
		{
			m_widget->setText(message, isError, showLoader);
		}
	);
}

void QtStatusBarView::setErrorCount(ErrorCountInfo errorCount)
{
	m_onQtThread(
		[=]()
		{
			m_widget->setErrorCount(errorCount);
		}
	);
}

void QtStatusBarView::showIdeStatus(const std::wstring& message)
{
	m_onQtThread(
		[=]()
		{
			m_widget->setIdeStatus(message);
		}
	);
}

void QtStatusBarView::showIndexingProgress(size_t progressPercent)
{
	m_onQtThread(
		[=]()
		{
			m_widget->showIndexingProgress(progressPercent);
		}
	);
}

void QtStatusBarView::hideIndexingProgress()
{
	m_onQtThread(
		[=]()
		{
			m_widget->hideIndexingProgress();
		}
	);
}
