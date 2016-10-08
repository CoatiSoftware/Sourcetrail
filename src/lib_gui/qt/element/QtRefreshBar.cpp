#include "qt/element/QtRefreshBar.h"

#include <QHBoxLayout>
#include <QPushButton>

#include "utility/messaging/type/MessageRefresh.h"

#include "utility/ResourcePaths.h"

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

	m_refreshButton->setEnabled(false);

	connect(m_refreshButton, SIGNAL(clicked()), this, SLOT(refreshClicked()));

	refreshStyle();
}

QtRefreshBar::~QtRefreshBar()
{
}

void QtRefreshBar::refreshClicked()
{
	MessageRefresh().dispatch();
}

void QtRefreshBar::refreshStyle()
{
	float height = std::max(ApplicationSettings::getInstance()->getFontSize() + 16, 30);

	m_refreshButton->setFixedHeight(height);

	std::string map = ResourcePaths::getGuiPath() + "refresh_view/images/refresh.png";
	m_refreshButton->setIcon(utility::colorizePixmap(
		QPixmap(map.c_str()),
		ColorScheme::getInstance()->getColor("search/button/icon").c_str()
	));
}

void QtRefreshBar::handleMessage(MessageEnteredLicense* message)
{
	m_onQtThread(
		[=]()
		{
			m_refreshButton->setEnabled(true);
		}
	);
}
