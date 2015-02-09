#include "qt/element/QtRefreshBar.h"

#include <QHBoxLayout>
#include <QPushButton>

#include "utility/messaging/type/MessageAutoRefreshChanged.h"
#include "utility/messaging/type/MessageRefresh.h"

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
	m_refreshButton->setIcon(QIcon("data/gui/refresh_view/images/refresh.png"));
	layout->addWidget(m_refreshButton);

	m_autoRefreshButton = new QPushButton(this);
	m_autoRefreshButton->setObjectName("auto_refresh_button");
	m_autoRefreshButton->setCheckable(true);
	m_autoRefreshButton->setToolTip("automatic refresh on window focus");
	m_autoRefreshButton->setAttribute(Qt::WA_LayoutUsesWidgetRect); // fixes layouting on Mac
	m_autoRefreshButton->setIcon(QIcon("data/gui/refresh_view/images/auto_refresh.png"));
	layout->addWidget(m_autoRefreshButton);

	connect(m_refreshButton, SIGNAL(clicked()), this, SLOT(refreshClicked()));
	connect(m_autoRefreshButton, SIGNAL(clicked()), this, SLOT(autoRefreshClicked()));
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
