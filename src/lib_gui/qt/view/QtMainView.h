#ifndef QT_MAIN_VIEW_H
#define QT_MAIN_VIEW_H

#include <memory>
#include <vector>

#include <QStatusBar>

#include "component/view/MainView.h"
#include "qt/utility/QtThreadedFunctor.h"

class QtMainWindow;
class View;

class QtMainView
	: public MainView
{
public:
	QtMainView();
	virtual ~QtMainView();

	// ViewLayout implementation
	virtual void addView(View* view);
	virtual void removeView(View* view);

	virtual void showView(View* view);
	virtual void hideView(View* view);

	virtual void loadLayout();
	virtual void saveLayout();

	virtual QStatusBar* getStatusBar();
	virtual void setStatusBar(QStatusBar* statusBar);

	// MainView implementation
	virtual void hideStartScreen();
	virtual void setTitle(const std::string& title);
	virtual void activateWindow();
	virtual void updateRecentProjectMenu();
	virtual void showLicenseScreen();

private:
	void doSetTitle(const std::string& title);
	void doActivateWindow();
	void doUpdateRecentProjectMenu();
	void doShowLicenseScreen();

	std::shared_ptr<QtMainWindow> m_window;
	std::vector<View*> m_views;

	QtThreadedFunctor<const std::string&> m_setTitleFunctor;
	QtThreadedFunctor<> m_activateWindowFunctor;
	QtThreadedFunctor<> m_updateRecentProjectMenuFunctor;
	QtThreadedFunctor<> m_showLicenseScreenFunctor;
};

#endif // QT_MAIN_VIEW_H
