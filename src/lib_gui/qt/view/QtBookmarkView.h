#ifndef QT_BOOKMARK_VIEW_H
#define QT_BOOKMARK_VIEW_H

#include "component/view/BookmarkView.h"

#include "qt/utility/QtThreadedFunctor.h"

class QFrame;
class QPushButton;
class QtBookmarkBrowser;

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

	virtual void setCreateButtonState(const CreateButtonState& state);
	virtual void enableDisplayBookmarks(bool enable);

	virtual bool bookmarkBrowserIsVisible() const;

private slots:
	void createBookmarkClicked();
	void showBookmarksClicked();

private:
	virtual void displayBookmarks(const std::vector<std::shared_ptr<Bookmark>>& bookmarks);
	virtual void displayBookmarkCreator(const std::vector<std::string>& names, const std::vector<BookmarkCategory>& categories, Id nodeId);
	virtual void displayBookmarkEditor(std::shared_ptr<Bookmark> bookmark, const std::vector<BookmarkCategory>& categories);

	void setStyleSheet();
	void refreshStyle();

	QtThreadedLambdaFunctor m_onQtThread;

	QFrame* m_widget;

	QPushButton* m_createBookmarkButton;
	QPushButton* m_showBookmarksButton;

	QtBookmarkBrowser* m_bookmarkBrowser;

	BookmarkView::CreateButtonState m_createButtonState;
};

#endif // QT_BOOKMARK_VIEW_H
