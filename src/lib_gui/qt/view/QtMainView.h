#ifndef QT_MAIN_VIEW_H
#define QT_MAIN_VIEW_H

#include <memory>
#include <vector>

#include <QStatusBar>

#include "component/view/MainView.h"
#include "qt/utility/QtThreadedFunctor.h"

#include "utility/messaging/MessageListener.h"
#include "utility/messaging/type/MessageForceEnterLicense.h"
#include "utility/messaging/type/MessageLoadProject.h"
#include "utility/messaging/type/MessageProjectEdit.h"
#include "utility/messaging/type/MessageProjectNew.h"

class QtMainWindow;
class View;

class QtMainView
	: public MainView
	, public MessageListener<MessageForceEnterLicense>
	, public MessageListener<MessageLoadProject>
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

	virtual View* findFloatingView(const std::string& name) const;

	virtual QStatusBar* getStatusBar();
	virtual void setStatusBar(QStatusBar* statusBar);

	// MainView implementation
	virtual void loadLayout();
	virtual void saveLayout();

	virtual void refreshView();

	virtual void hideStartScreen();
	virtual void setTitle(const std::wstring& title);
	virtual void activateWindow();

	virtual void updateRecentProjectMenu();
	virtual void updateHistoryMenu(const std::vector<SearchMatch>& history);
	virtual void updateBookmarksMenu(const std::vector<std::shared_ptr<Bookmark>>& bookmarks);

private:
	void handleMessage(MessageForceEnterLicense* message);
	void handleMessage(MessageLoadProject* message);
	void handleMessage(MessageProjectEdit* message);
	void handleMessage(MessageProjectNew* message);

	std::shared_ptr<QtMainWindow> m_window;
	std::vector<View*> m_views;

	QtThreadedLambdaFunctor m_onQtThread;
};

#endif // QT_MAIN_VIEW_H
