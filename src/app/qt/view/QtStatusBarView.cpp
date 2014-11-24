#include "qt/view/QtStatusBarView.h"

#include <QStatusBar>
#include "qt/view/QtMainView.h"
#include "qt/view/QtViewWidgetWrapper.h"

QtStatusBarView::QtStatusBarView( ViewLayout* viewLayout )
	: StatusBarView(viewLayout)
	, m_showMessageFunctor(std::bind(&QtStatusBarView::doShowMessage, this, std::placeholders::_1, std::placeholders::_2))
{
	QtMainView* mw = static_cast<QtMainView*>(viewLayout);
	m_widget = std::make_shared<QtStatusBar>();
	QStatusBar* sb = static_cast<QStatusBar*>(m_widget.get());
	mw->setStatusBar(sb);
	m_widget->show();
}

QtStatusBarView::~QtStatusBarView()
{
}

void QtStatusBarView::createWidgetWrapper()
{
	//setWidgetWrapper(std::make_shared<QtViewWidgetWrapper>(m_widget));
}

void QtStatusBarView::initView()
{

}

void QtStatusBarView::refreshView()
{

}

void QtStatusBarView::doShowMessage( const std::string& message, bool isError )
{
	m_widget->setText(message, isError);
}

void QtStatusBarView::showMessage( const std::string& message, bool isError )
{
	m_showMessageFunctor(message, isError);
}
