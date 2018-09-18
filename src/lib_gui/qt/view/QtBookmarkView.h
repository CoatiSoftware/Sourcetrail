#ifndef QT_BOOKMARK_VIEW_H
#define QT_BOOKMARK_VIEW_H

#include "BookmarkController.h"
#include "ControllerProxy.h"
#include "BookmarkView.h"

#include "QtThreadedFunctor.h"

class QFrame;
class QtBookmarkBrowser;
class QtSearchBarButton;

class QtBookmarkView
	: public QObject
	, public BookmarkView
{
	Q_OBJECT

public:
	QtBookmarkView(ViewLayout* viewLayout);
	virtual ~QtBookmarkView();

	// View implementation
	virtual void createWidgetWrapper();
	virtual void initView();
	virtual void refreshView();

	// BookmarkView implementation
	virtual void setCreateButtonState(const CreateButtonState& state);

	virtual void displayBookmarkCreator(
		const std::vector<std::wstring>& names, const std::vector<BookmarkCategory>& categories, Id nodeId);
	virtual void displayBookmarkEditor(
		std::shared_ptr<Bookmark> bookmark, const std::vector<BookmarkCategory>& categories);

	virtual void displayBookmarks(const std::vector<std::shared_ptr<Bookmark>>& bookmarks);
	virtual void enableDisplayBookmarks(bool enable);

	virtual bool bookmarkBrowserIsVisible() const;

private slots:
	void createBookmarkClicked();
	void showBookmarksClicked();

private:
	ControllerProxy<BookmarkController> m_controllerProxy;
	QtThreadedLambdaFunctor m_onQtThread;

	QFrame* m_widget;

	QtSearchBarButton* m_createBookmarkButton;
	QtSearchBarButton* m_showBookmarksButton;

	QtBookmarkBrowser* m_bookmarkBrowser;

	BookmarkView::CreateButtonState m_createButtonState;
};

#endif // QT_BOOKMARK_VIEW_H
