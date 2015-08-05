#include "qt/element/QtRefreshBar.h"

#include <QHBoxLayout>
#include <QPushButton>

#include "utility/messaging/type/MessageAutoRefreshChanged.h"
#include "utility/messaging/type/MessageRefresh.h"

#include "qt/utility/utilityQt.h"
#include "settings/ApplicationSettings.h"
#include "settings/ColorScheme.h"

QtRefreshBar::QtRefreshBar()
{
	setObjectName("refresh_bar");

	QBoxLayout* layout = new QHBoxLayout();
	layout->setSpacing(0);
	layout->setContentsMargins(0, 0, 0, 0);
	layout->setAlignment(Qt::AlignTop);
	setLayout(layout);

	m_refreshButton = new QPushButton(this);
	m_refreshButton->setObjectName("refresh_button");
	m_refreshButton->setToolTip("refresh");
	m_refreshButton->setAttribute(Qt::WA_LayoutUsesWidgetRect); // fixes layouting on Mac
	layout->addWidget(m_refreshButton);

	m_autoRefreshButton = new QPushButton(this);
	m_autoRefreshButton->setObjectName("auto_refresh_button");
	m_autoRefreshButton->setCheckable(true);
	m_autoRefreshButton->setToolTip("automatic refresh on window focus");
	m_autoRefreshButton->setAttribute(Qt::WA_LayoutUsesWidgetRect); // fixes layouting on Mac
	layout->addWidget(m_autoRefreshButton);

	connect(m_refreshButton, SIGNAL(clicked()), this, SLOT(refreshClicked()));
	connect(m_autoRefreshButton, SIGNAL(clicked()), this, SLOT(autoRefreshClicked()));

	refreshStyle();
}

QtRefreshBar::~QtRefreshBar()
{
}

void QtRefreshBar::refreshClicked()
{
	MessageRefresh().dispatch();
}

void QtRefreshBar::autoRefreshClicked()
{
	MessageAutoRefreshChanged(m_autoRefreshButton->isChecked()).dispatch();
}

void QtRefreshBar::refreshStyle()
{
	float height = std::max(ApplicationSettings::getInstance()->getFontSize() + 16, 30);

	m_refreshButton->setFixedHeight(height);
	m_autoRefreshButton->setFixedHeight(height);

	m_refreshButton->setIcon(utility::colorizePixmap(
		QPixmap("data/gui/refresh_view/images/refresh.png"),
		ColorScheme::getInstance()->getColor("search/button/icon").c_str()
	));

	m_autoRefreshButton->setIcon(utility::colorizePixmap(
		QPixmap("data/gui/refresh_view/images/auto_refresh.png"),
		ColorScheme::getInstance()->getColor("search/button/icon").c_str()
	));
}
