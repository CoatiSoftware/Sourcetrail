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
	~QtBookmarkView() = default;

	// View implementation
	void createWidgetWrapper() override;
	void refreshView() override;

	// BookmarkView implementation
	void displayBookmarkCreator(
		const std::vector<std::wstring>& names, const std::vector<BookmarkCategory>& categories, Id nodeId) override;
	void displayBookmarkEditor(
		std::shared_ptr<Bookmark> bookmark, const std::vector<BookmarkCategory>& categories) override;

	void displayBookmarks(const std::vector<std::shared_ptr<Bookmark>>& bookmarks) override;
	bool bookmarkBrowserIsVisible() const override;

private:
	ControllerProxy<BookmarkController> m_controllerProxy;
	QtThreadedLambdaFunctor m_onQtThread;

	QtBookmarkBrowser* m_bookmarkBrowser;
};

#endif // QT_BOOKMARK_VIEW_H
