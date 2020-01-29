#include "QtRefreshView.h"

#include <QFrame>
#include <QHBoxLayout>

#include "MessageIndexingShowDialog.h"
#include "MessageRefresh.h"
#include "ResourcePaths.h"

#include "QtSearchBarButton.h"
#include "QtViewWidgetWrapper.h"
#include "utilityQt.h"

QtRefreshView::QtRefreshView(ViewLayout* viewLayout): RefreshView(viewLayout)
{
	m_widget = new QFrame();
	m_widget->setObjectName(QStringLiteral("refresh_bar"));

	QBoxLayout* layout = new QHBoxLayout();
	layout->setSpacing(0);
	layout->setContentsMargins(0, 0, 0, 0);
	layout->setAlignment(Qt::AlignTop);

	QtSearchBarButton* refreshButton = new QtSearchBarButton(
		ResourcePaths::getGuiPath().concatenate(L"refresh_view/images/refresh.png"));
	refreshButton->setObjectName(QStringLiteral("refresh_button"));
	refreshButton->setToolTip(QStringLiteral("refresh"));
	m_widget->connect(refreshButton, &QPushButton::clicked, []() {
		MessageIndexingShowDialog().dispatch();
		MessageRefresh().dispatch();
	});

	layout->addWidget(refreshButton);
	m_widget->setLayout(layout);
}

void QtRefreshView::createWidgetWrapper()
{
	setWidgetWrapper(std::make_shared<QtViewWidgetWrapper>(m_widget));
}

void QtRefreshView::refreshView()
{
	m_onQtThread([this]() {
		m_widget->setStyleSheet(utility::getStyleSheet(ResourcePaths::getGuiPath().concatenate(
														   L"refresh_view/refresh_view.css"))
									.c_str());
	});
}
