#ifndef QT_MAIN_VIEW_H
#define QT_MAIN_VIEW_H

#include <memory>
#include <vector>

#include <QStatusBar>

#include "MainView.h"
#include "QtThreadedFunctor.h"

#include "MessageListener.h"
#include "MessageProjectEdit.h"
#include "MessageProjectNew.h"

class QtMainWindow;
class View;

class QtMainView
	: public MainView
	, public MessageListener<MessageProjectEdit>
	, public MessageListener<MessageProjectNew>
{
public:
	QtMainView(const ViewFactory* viewFactory, StorageAccess* storageAccess);
	~QtMainView();

	QtMainWindow* getMainWindow() const;

	// ViewLayout implementation
	void addView(View* view) override;
	void overrideView(View* view) override;
	void removeView(View* view) override;

	void showView(View* view) override;
	void hideView(View* view) override;

	void setViewEnabled(View* view, bool enabled) override;

	View* findFloatingView(const std::string& name) const override;

	void showOriginalViews() override;

	QStatusBar* getStatusBar();
	void setStatusBar(QStatusBar* statusBar);

	// MainView implementation
	void loadLayout() override;
	void saveLayout() override;

	void loadWindow(bool showStartWindow) override;

	void refreshView() override;
	void refreshUIState(bool isAfterIndexing) override;

	void hideStartScreen() override;
	void setTitle(const std::wstring& title) override;
	void activateWindow() override;

	void updateRecentProjectMenu() override;

	void updateHistoryMenu(std::shared_ptr<MessageBase> message) override;
	void clearHistoryMenu() override;

	void updateBookmarksMenu(const std::vector<std::shared_ptr<Bookmark>>& bookmarks) override;
	void clearBookmarksMenu() override;

private:
	void handleMessage(MessageProjectEdit* message) override;
	void handleMessage(MessageProjectNew* message) override;

	std::shared_ptr<QtMainWindow> m_window;
	std::vector<View*> m_views;

	QtThreadedLambdaFunctor m_onQtThread;
};

#endif	  // QT_MAIN_VIEW_H
