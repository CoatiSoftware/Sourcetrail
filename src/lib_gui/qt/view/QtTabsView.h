#ifndef QT_TABS_VIEW_H
#define QT_TABS_VIEW_H

#include <QObject>

#include "QtThreadedFunctor.h"
#include "TabsController.h"
#include "TabsView.h"

class QtTabBar;

class QtTabsView
	: public QObject
	, public TabsView
{
	Q_OBJECT

public:
	QtTabsView(ViewLayout* viewLayout);
	virtual ~QtTabsView() = default;

	// View implementation
	void createWidgetWrapper() override;
	void refreshView() override;

	// TabsView implementation
	void clear() override;
	void openTab(bool showTab, SearchMatch match) override;
	void closeTab() override;
	void destroyTab(Id tabId) override;
	void selectTab(bool next) override;
	void updateTab(Id tabId, std::vector<SearchMatch> matches) override;

private slots:
	void addTab();
	void insertTab(bool showTab, SearchMatch match);
	void changedTab(int index);
	void removeTab(int index);
	void closeTabsToRight(int index);

private:
	void setTabState(int idx, const std::vector<SearchMatch>& matches);

	void setStyleSheet();

	QtThreadedLambdaFunctor m_onQtThread;

	QWidget* m_widget;
	QtTabBar* m_tabBar;

	size_t m_insertedTabCount;
};

#endif	  // QT_TABS_VIEW_H
