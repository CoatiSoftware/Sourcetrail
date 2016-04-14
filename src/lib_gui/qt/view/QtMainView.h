#ifndef QT_MAIN_VIEW_H
#define QT_MAIN_VIEW_H

#include <memory>
#include <vector>

#include <QStatusBar>

#include "component/view/MainView.h"
#include "qt/utility/QtThreadedFunctor.h"

#include "utility/messaging/MessageListener.h"
#include "utility/messaging/type/MessageProjectNew.h"
#include "utility/messaging/type/MessageShowStartScreen.h"

class QtMainWindow;
class View;

class QtMainView
	: public MainView
	, public MessageListener<MessageProjectNew>
	, public MessageListener<MessageShowStartScreen>
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
	virtual void forceLicenseScreen();

private:
	void handleMessage(MessageProjectNew* message);
	void handleMessage(MessageShowStartScreen* message);

	void doCreateNewProjectFromSolution(const std::string& ideId, const std::string& solutionPath);
	void doShowStartScreen();
	void doHideStartScreen();
	void doSetTitle(const std::string& title);
	void doActivateWindow();
	void doUpdateRecentProjectMenu();
	void doForceLicenseScreen();

	std::shared_ptr<QtMainWindow> m_window;
	std::vector<View*> m_views;

	QtThreadedFunctor<const std::string&, const std::string&> m_createNewProjectFromSolutionFunctor;
	QtThreadedFunctor<> m_showStartScreenFunctor;
	QtThreadedFunctor<> m_hideStartScreenFunctor;
	QtThreadedFunctor<const std::string&> m_setTitleFunctor;
	QtThreadedFunctor<> m_activateWindowFunctor;
	QtThreadedFunctor<> m_updateRecentProjectMenuFunctor;
	QtThreadedFunctor<> m_forceLicenseScreenFunctor;
};

#endif // QT_MAIN_VIEW_H
