#ifndef MAIN_VIEW_H
#define MAIN_VIEW_H

#include <memory>
#include <string>
#include <vector>

#include "component/view/ViewLayout.h"

struct SearchMatch;
class Bookmark;
class MessageBase;

class MainView
	: public ViewLayout
{
public:
	MainView();
	virtual ~MainView();

	virtual void loadLayout() = 0;
	virtual void saveLayout() = 0;

	virtual void refreshView() = 0;

	virtual void hideStartScreen() = 0;
	virtual void setTitle(const std::wstring& title) = 0;
	virtual void activateWindow() = 0;

	virtual void updateRecentProjectMenu() = 0;
	virtual void updateHistoryMenu(const std::vector<std::shared_ptr<MessageBase>>& historyMenuItems) = 0;
	virtual void updateBookmarksMenu(const std::vector<std::shared_ptr<Bookmark>>& bookmarks) = 0;
};

#endif // MAIN_VIEW_H
