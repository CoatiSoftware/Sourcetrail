#include "QtTabsView.h"

#include <QHBoxLayout>
#include <QPushButton>
#include <QStyle>
#include <QVariant>

#include "Application.h"
#include "ColorScheme.h"
#include "GraphViewStyle.h"
#include "QtSelfRefreshIconButton.h"
#include "QtTabBar.h"
#include "QtViewWidgetWrapper.h"
#include "ResourcePaths.h"
#include "TabId.h"
#include "TabsController.h"
#include "utilityQt.h"

QtTabsView::QtTabsView(ViewLayout* viewLayout)
	: TabsView(viewLayout), m_widget(nullptr), m_insertedTabCount(0)
{
	m_widget = new QWidget();

	QHBoxLayout* layout = new QHBoxLayout(m_widget);
	layout->setContentsMargins(0, 0, 0, 0);
	layout->setSpacing(0);

	QWidget* front = new QWidget();
	front->setMinimumWidth(5);
	front->setObjectName(QStringLiteral("side_area"));
	layout->addWidget(front);

	m_tabBar = new QtTabBar();
	m_tabBar->setDrawBase(false);
	m_tabBar->setMinimumWidth(0);
	m_tabBar->setMovable(true);
	m_tabBar->setElideMode(Qt::ElideMiddle);
	layout->addWidget(m_tabBar);

	connect(m_tabBar, &QTabBar::currentChanged, this, &QtTabsView::changedTab);

	QPushButton* addButton = new QtSelfRefreshIconButton(
		QLatin1String(""), ResourcePaths::getGuiPath().concatenate(L"tabs_view/images/add.png"), "tab/bar/button");
	addButton->setObjectName(QStringLiteral("add_button"));
	addButton->setIconSize(QSize(14, 14));

	QWidget* back = new QWidget();
	back->setObjectName(QStringLiteral("side_area"));
	QHBoxLayout* backLayout = new QHBoxLayout(back);
	backLayout->setContentsMargins(3, 0, 5, 0);
	backLayout->setSpacing(0);
	backLayout->addWidget(addButton);
	backLayout->addStretch();
	layout->addWidget(back);

	connect(addButton, &QPushButton::clicked, this, &QtTabsView::addTab);
	connect(m_tabBar, &QtTabBar::signalCloseTabsToRight, this, &QtTabsView::closeTabsToRight);
}

void QtTabsView::createWidgetWrapper()
{
	setWidgetWrapper(std::make_shared<QtViewWidgetWrapper>(m_widget));
}

void QtTabsView::refreshView()
{
	m_onQtThread([=]() { setStyleSheet(); });
}

void QtTabsView::clear()
{
	m_onQtThread([=]() {
		getController<TabsController>()->onClearTabs();

		m_tabBar->blockSignals(true);

		int c = m_tabBar->count();
		for (int i = c - 1; i >= 0; i--)
		{
			removeTab(i);
		}

		m_tabBar->blockSignals(false);
	});
}

void QtTabsView::openTab(bool showTab, SearchMatch match)
{
	m_onQtThread([=]() { insertTab(showTab, match); });
}

void QtTabsView::closeTab()
{
	m_onQtThread([=]() { removeTab(m_tabBar->currentIndex()); });
}

void QtTabsView::destroyTab(Id tabId)
{
	m_onQtThread([=]() { getController<TabsController>()->destroyTab(tabId); });
}

void QtTabsView::selectTab(bool next)
{
	m_onQtThread([=]() {
		int idx = m_tabBar->currentIndex();
		if (idx != -1)
		{
			idx += next ? 1 : -1;
			m_tabBar->setCurrentIndex((idx + m_tabBar->count()) % m_tabBar->count());
		}
	});
}

void QtTabsView::updateTab(Id tabId, std::vector<SearchMatch> matches)
{
	m_onQtThread([=]() {
		for (int i = 0; i < m_tabBar->count(); i++)
		{
			if (m_tabBar->tabData(i).toInt() == int(tabId))
			{
				setTabState(i, matches);
				return;
			}
		}
	});
}

void QtTabsView::addTab()
{
	if (Application::getInstance()->isProjectLoaded())
	{
		insertTab(true, SearchMatch());
	}
}

void QtTabsView::insertTab(bool showTab, SearchMatch match)
{
	int tabId = TabId::nextTab();

	m_tabBar->blockSignals(true);

	m_insertedTabCount++;
	int idx = match.isValid() ? m_tabBar->currentIndex() + m_insertedTabCount : m_tabBar->count() + 1;
	idx = m_tabBar->insertTab(idx, QStringLiteral(" Empty Tab "));
	m_tabBar->setTabData(idx, QVariant(tabId));

	QPushButton* typeCircle = new QPushButton();
	typeCircle->setObjectName(QStringLiteral("type_circle"));
	m_tabBar->setTabButton(idx, QTabBar::LeftSide, typeCircle);

	connect(typeCircle, &QPushButton::clicked, [tabId, this]() {
		for (int i = 0; i < m_tabBar->count(); i++)
		{
			if (m_tabBar->tabData(i).toInt() == tabId)
			{
				m_tabBar->setCurrentIndex(i);
				return;
			}
		}
	});

	QPushButton* closeButton = new QtSelfRefreshIconButton(
		QLatin1String(""),
		ResourcePaths::getGuiPath().concatenate(L"tabs_view/images/close.png"),
		"tab/bar/button");
	closeButton->setObjectName(QStringLiteral("close_button"));
	closeButton->setIconSize(QSize(10, 10));
	m_tabBar->setTabButton(idx, QTabBar::RightSide, closeButton);

	connect(closeButton, &QPushButton::clicked, [tabId, this]() {
		for (int i = 0; i < m_tabBar->count(); i++)
		{
			if (m_tabBar->tabData(i).toInt() == tabId)
			{
				removeTab(i);
				return;
			}
		}
	});

	m_tabBar->blockSignals(false);

	getController<TabsController>()->addTab(tabId, match);

	if (m_tabBar->count() == 1)
	{
		changedTab(m_tabBar->currentIndex());
	}
	else if (showTab)
	{
		m_tabBar->setCurrentIndex(idx);
	}

	setTabState(idx, {});
}

void QtTabsView::changedTab(int index)
{
	m_insertedTabCount = 0;

	getController<TabsController>()->showTab(m_tabBar->tabData(index).toInt());

	for (int i = 0; i < m_tabBar->count(); i++)
	{
		QWidget* circle = m_tabBar->tabButton(i, QTabBar::LeftSide);
		bool selected = (i == index);
		if (circle->property("selected").toBool() != selected)
		{
			circle->setProperty("selected", selected);
			circle->style()->unpolish(circle);
			circle->style()->polish(circle);
		}
	}
}

void QtTabsView::removeTab(int index)
{
	m_insertedTabCount = 0;

	getController<TabsController>()->removeTab(m_tabBar->tabData(index).toInt());
	m_tabBar->removeTab(index);
}

void QtTabsView::setTabState(int idx, const std::vector<SearchMatch>& matches)
{
	ColorScheme* scheme = ColorScheme::getInstance().get();
	std::wstring name;
	std::string color;
	std::string activeColor;

	if (matches.size())
	{
		const SearchMatch& match = matches[0];
		name = match.getFullName();

		if (match.searchType == SearchMatch::SEARCH_TOKEN)
		{
			color = GraphViewStyle::getNodeColor(match.nodeType.getUnderscoredTypeString(), false).fill;
			activeColor =
				GraphViewStyle::getNodeColor(match.nodeType.getUnderscoredTypeString(), true).fill;
		}
		else
		{
			color = scheme->getSearchTypeColor(
				utility::encodeToUtf8(match.getSearchTypeName()), "fill");
			activeColor = scheme->getSearchTypeColor(
				utility::encodeToUtf8(match.getSearchTypeName()), "fill", "hover");
		}
	}
	else
	{
		name = L"Empty Tab";
		color = scheme->getColor("tab/bar/button/background/press");
		activeColor = color;
	}

	m_tabBar->setTabText(idx, ' ' + QString::fromStdWString(name) + ' ');
	m_tabBar->tabButton(idx, QTabBar::LeftSide)
		->setStyleSheet(
			QStringLiteral("#type_circle { background-color: ") + QString::fromStdString(color) +
			QStringLiteral("; } "
			"#type_circle[selected=true] { background-color: ") +
			QString::fromStdString(activeColor) + QStringLiteral("; } "));
}

void QtTabsView::setStyleSheet()
{
	const std::string css = utility::getStyleSheet(
		ResourcePaths::getGuiPath().concatenate(L"tabs_view/tabs_view.css"));
	m_widget->setStyleSheet(css.c_str());

	utility::setWidgetBackgroundColor(
		m_widget, ColorScheme::getInstance()->getColor("tab/bar/background"));
}

void QtTabsView::closeTabsToRight(int tabNum)
{
	LOG_INFO("Closing tabs to the right of tab nr. " + std::to_string(tabNum));
	// We are closing tabs to the right, hence the increase.
	tabNum++;  
	// Now close tabs at position tabNum until count has decreased low enough.
	while(tabNum < m_tabBar->count() )
	{
		m_tabBar->removeTab(tabNum);
	}
}

