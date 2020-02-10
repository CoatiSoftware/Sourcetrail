#ifndef MAIN_VIEW_H
#define MAIN_VIEW_H

#include <memory>
#include <string>
#include <vector>

#include "ComponentManager.h"
#include "DialogView.h"
#include "ViewLayout.h"

class Bookmark;
class MessageBase;
class StorageAccess;
class ViewFactory;

class MainView: public ViewLayout
{
public:
	MainView(const ViewFactory* viewFactory, StorageAccess* storageAccess);
	virtual ~MainView() = default;

	void setup();
	void clear();
	void refreshViews();

	std::shared_ptr<DialogView> getDialogView(DialogView::UseCase useCase);

	virtual void loadLayout() = 0;
	virtual void saveLayout() = 0;

	virtual void refreshView() = 0;
	virtual void refreshUIState(bool isAfterIndexing) = 0;

	virtual void loadWindow(bool showStartWindow) = 0;

	virtual void hideStartScreen() = 0;
	virtual void setTitle(const std::wstring& title) = 0;
	virtual void activateWindow() = 0;

	virtual void updateRecentProjectMenu() = 0;

	virtual void updateHistoryMenu(std::shared_ptr<MessageBase> message) = 0;
	virtual void clearHistoryMenu() = 0;

	virtual void updateBookmarksMenu(const std::vector<std::shared_ptr<Bookmark>>& bookmarks) = 0;
	virtual void clearBookmarksMenu() = 0;

protected:
	ComponentManager m_componentManager;
};

#endif	  // MAIN_VIEW_H
