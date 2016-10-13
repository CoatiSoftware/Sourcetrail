#include "qt/view/QtLogView.h"

#include <QBoxLayout>
#include <QFrame>
#include <QLabel>

#include "qt/view/QtViewWidgetWrapper.h"

QtLogView::QtLogView(ViewLayout* viewLayout)
	: LogView(viewLayout)
	, m_clearFunctor(std::bind(&QtLogView::doClear, this))
	, m_refreshFunctor(std::bind(&QtLogView::doRefreshView, this))
{
}

QtLogView::~QtLogView()
{
}

void QtLogView::createWidgetWrapper()
{
	setWidgetWrapper(std::make_shared<QtViewWidgetWrapper>(new QFrame()));
}

void QtLogView::initView()
{
	QWidget* widget = QtViewWidgetWrapper::getWidgetOfView(this);

	QBoxLayout* layout = new QBoxLayout(QBoxLayout::TopToBottom);
	layout->setContentsMargins(0, 0, 0, 0);
	layout->setSpacing(0);
	widget->setLayout(layout);

	QLabel* label = new QLabel("test log view");

	widget->layout()->addWidget(label);

	doRefreshView();
}

void QtLogView::refreshView()
{
	m_refreshFunctor();
}

void QtLogView::clear()
{
	m_clearFunctor();
}

void QtLogView::doClear()
{
}

void QtLogView::doRefreshView()
{
}
