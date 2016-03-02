#include "qt/view/QtStatusBarView.h"

#include <QStatusBar>

#include "qt/view/QtMainView.h"
#include "qt/view/QtViewWidgetWrapper.h"

QtStatusBarView::QtStatusBarView(ViewLayout* viewLayout)
	: StatusBarView(viewLayout)
	, m_showMessageFunctor(std::bind(
		&QtStatusBarView::doShowMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3))
	, m_setErrorCountFunctor(std::bind(&QtStatusBarView::doSetErrorCount, this, std::placeholders::_1))
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

void QtStatusBarView::showMessage(const std::string& message, bool isError, bool showLoader)
{
	m_showMessageFunctor(message, isError, showLoader);
}

void QtStatusBarView::setErrorCount(ErrorCountInfo errorCount)
{
	m_setErrorCountFunctor(errorCount);
}

void QtStatusBarView::doShowMessage(const std::string& message, bool isError, bool showLoader)
{
	m_widget->setText(message, isError, showLoader);
}

void QtStatusBarView::doSetErrorCount(ErrorCountInfo errorCount)
{
	m_widget->setErrorCount(errorCount);
}
