#ifndef QT_BOOKMARK_VIEW_H
#define QT_BOOKMARK_VIEW_H

#include "BookmarkController.h"
#include "BookmarkView.h"
#include "ControllerProxy.h"
#include "QtThreadedFunctor.h"

class QFrame;
class QtBookmarkBrowser;

class QtBookmarkView
	: public BookmarkView
{
public:
	QtBookmarkView(ViewLayout* viewLayout);
	virtual ~QtBookmarkView() = default;

	// View implementation
	virtual void createWidgetWrapper();
	virtual void initView();
	virtual void refreshView();

	// BookmarkView implementation
	virtual void displayBookmarkCreator(
		const std::vector<std::wstring>& names, const std::vector<BookmarkCategory>& categories, Id nodeId);
	virtual void displayBookmarkEditor(
		std::shared_ptr<Bookmark> bookmark, const std::vector<BookmarkCategory>& categories);

	virtual void displayBookmarks(const std::vector<std::shared_ptr<Bookmark>>& bookmarks);
	virtual bool bookmarkBrowserIsVisible() const;

private:
	ControllerProxy<BookmarkController> m_controllerProxy;
	QtThreadedLambdaFunctor m_onQtThread;

	QtBookmarkBrowser* m_bookmarkBrowser;
};

#endif // QT_BOOKMARK_VIEW_H
