#ifndef QT_BOOKMARK_VIEW_H
#define QT_BOOKMARK_VIEW_H

#include "component/view/BookmarkView.h"

#include "qt/element/QtBookmarkBar.h"
#include "qt/utility/QtThreadedFunctor.h"

class QtBookmarkView
	: public BookmarkView
{
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
	
private:
	void doRefreshView();

	void setStyleSheet();

	virtual void displayBookmarks(const std::vector<std::shared_ptr<Bookmark>>& bookmarks);
	virtual void displayBookmarkCreator(const std::vector<std::string>& names, const std::vector<BookmarkCategory>& categories);
	virtual void displayBookmarkEditor(std::shared_ptr<Bookmark> bookmark, const std::vector<BookmarkCategory>& categories);

	QtThreadedFunctor<> m_refreshViewFunctor;

	QtBookmarkBar* m_widget;
};

#endif // QT_BOOKMARK_VIEW_H