#include "QtTabbedView.h"

#include <QEvent>
#include <QFrame>
#include <QTabBar>
#include <QTabWidget>
#include <QVBoxLayout>

#include "ColorScheme.h"
#include "QtSelfRefreshIconButton.h"
#include "QtViewWidgetWrapper.h"
#include "ResourcePaths.h"
#include "utilityQt.h"

QtTabbedView::QtTabbedView(ViewLayout* viewLayout, const std::string& name)
	: TabbedView(viewLayout, name)
{
	setWidgetWrapper(std::make_shared<QtViewWidgetWrapper>(new QFrame()));
	QWidget* widget = QtViewWidgetWrapper::getWidgetOfView(this);

	QVBoxLayout* layout = new QVBoxLayout();
	layout->setContentsMargins(0, 0, 0, 0);
	layout->setSpacing(0);
	widget->setLayout(layout);

	m_widget = new QTabWidget(widget);
	layout->addWidget(m_widget);

	m_closeButton = new QtSelfRefreshIconButton(
		QLatin1String(""),
		ResourcePaths::getGuiPath().concatenate(L"screen_search_view/images/close.png"),
		"screen_search/button",
		widget);
	m_closeButton->setIconSize(QSize(15, 15));
	m_closeButton->setStyleSheet(QStringLiteral("background: transparent; border: none;"));

	widget->connect(m_closeButton, &QPushButton::clicked, [this]() { hideView(this); });
	m_widget->tabBar()->installEventFilter(this);
	widget->installEventFilter(this);
}

void QtTabbedView::createWidgetWrapper() {}

void QtTabbedView::refreshView()
{
	m_onQtThread([=]() { setStyleSheet(); });
}

void QtTabbedView::addViewWidget(View* view)
{
	QWidget* tabWidget = QtViewWidgetWrapper::getWidgetOfView(view);
	tabWidget->setObjectName(QStringLiteral("tab_content"));
	m_widget->addTab(tabWidget, view->getName().c_str());

	setStyleSheet();
}

void QtTabbedView::showView(View* view)
{
	TabbedView::showView(view);

	m_widget->setCurrentWidget(QtViewWidgetWrapper::getWidgetOfView(view));
}

void QtTabbedView::setStyleSheet()
{
	utility::setWidgetBackgroundColor(
		QtViewWidgetWrapper::getWidgetOfView(this),
		ColorScheme::getInstance()->getColor("tab/background"));

	m_widget->setStyleSheet(utility::getStyleSheet(ResourcePaths::getGuiPath().concatenate(
													   L"tabbed_view/tabbed_view.css"))
								.c_str());
}

bool QtTabbedView::eventFilter(QObject* obj, QEvent* event)
{
	if (event->type() == QEvent::Resize)
	{
		m_closeButton->setGeometry(
			m_widget->width() - 23, (m_widget->tabBar()->height() - 15) / 2 + 1, 15, 15);
		m_closeButton->show();
	}
	return QObject::eventFilter(obj, event);
}
