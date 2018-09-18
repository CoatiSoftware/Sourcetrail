#ifndef QT_MAIN_VIEW_H
#define QT_MAIN_VIEW_H

#include <memory>
#include <vector>

#include <QStatusBar>

#include "MainView.h"
#include "QtThreadedFunctor.h"

#include "MessageListener.h"
#include "MessageForceEnterLicense.h"
#include "MessageProjectEdit.h"
#include "MessageProjectNew.h"

class QtMainWindow;
class View;

class QtMainView
	: public MainView
	, public MessageListener<MessageForceEnterLicense>
	, public MessageListener<MessageProjectEdit>
	, public MessageListener<MessageProjectNew>
{
public:
	QtMainView();
	virtual ~QtMainView();

	QtMainWindow* getMainWindow() const;

	// ViewLayout implementation
	virtual void addView(View* view);
	virtual void removeView(View* view);

	virtual void showView(View* view);
	virtual void hideView(View* view);

	virtual void setViewEnabled(View* view, bool enabled);

	virtual View* findFloatingView(const std::string& name) const;

	virtual QStatusBar* getStatusBar();
	virtual void setStatusBar(QStatusBar* statusBar);

	// MainView implementation
	virtual void loadLayout();
	virtual void saveLayout();

	virtual void loadWindow(bool showStartWindow);

	virtual void refreshView();

	virtual void hideStartScreen();
	virtual void setTitle(const std::wstring& title);
	virtual void activateWindow();

	virtual void updateRecentProjectMenu();
	virtual void updateHistoryMenu(const std::vector<std::shared_ptr<MessageBase>>& historyMenuItems);
	virtual void updateBookmarksMenu(const std::vector<std::shared_ptr<Bookmark>>& bookmarks);

private:
	void handleMessage(MessageForceEnterLicense* message);
	void handleMessage(MessageProjectEdit* message);
	void handleMessage(MessageProjectNew* message);

	std::shared_ptr<QtMainWindow> m_window;
	std::vector<View*> m_views;

	QtThreadedLambdaFunctor m_onQtThread;
};

#endif // QT_MAIN_VIEW_H
