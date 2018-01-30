#include "qt/view/QtStatusBarView.h"

#include <QStatusBar>

#include "qt/view/QtMainView.h"

QtStatusBarView::QtStatusBarView(ViewLayout* viewLayout)
	: StatusBarView(viewLayout)
{
	m_widget = std::make_shared<QtStatusBar>();
	m_widget->show();

	QtMainView* mw = static_cast<QtMainView*>(viewLayout);
	QStatusBar* sb = static_cast<QStatusBar*>(m_widget.get());
	mw->setStatusBar(sb);
}

QtStatusBarView::~QtStatusBarView()
{
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
