#ifndef QT_BOOKMARK_BAR_H
#define QT_BOOKMARK_BAR_H

#include <QFrame>
#include <qtextedit.h>

#include "data/bookmark/Bookmark.h"

#include "component/view/BookmarkView.h"
#include "qt/window/QtBookmarkBrowser.h"
#include "qt/utility/QtThreadedFunctor.h"
#include "utility/messaging/type/MessageEnteredLicense.h"
#include "utility/messaging/MessageListener.h"

#include "data/bookmark/Bookmark.h"

class QPushButton;

class QtBookmarkBar
	: public QFrame
	, public MessageListener<MessageEnteredLicense>
{
	Q_OBJECT

public:
	QtBookmarkBar();
	virtual ~QtBookmarkBar();

	void refreshStyle();

	virtual void displayBookmarks(const std::vector<std::shared_ptr<Bookmark>>& bookmarks);
	virtual void displayBookmarkCreator(const std::vector<std::string>& names, const std::vector<BookmarkCategory>& categories);
	virtual void displayBookmarkEditor(std::shared_ptr<Bookmark> bookmark, const std::vector<BookmarkCategory>& categories);

	void setCreateButtonState(const BookmarkView::CreateButtonState& state);
	void enableDisplayButton(bool enable);

	bool bookmarkBrowserIsVisible() const;

private slots:
	void createBookmarkClicked();
	void showBookmarksClicked();

private:
	virtual void handleMessage(MessageEnteredLicense* message);

	void doDisplayBookmarks(const std::vector<std::shared_ptr<Bookmark>>& bookmarks);
	void doDisplayBookmarkCreator(const std::vector<std::string>& names, const std::vector<BookmarkCategory>& categories);
	void doDisplayBookmarkEditor(std::shared_ptr<Bookmark> bookmark, const std::vector<BookmarkCategory>& categories);
	void doSetCreateButtonState(const BookmarkView::CreateButtonState& state);
	void doEnableDisplayButton(bool enable);

	QtThreadedLambdaFunctor m_onQtThread;

	QPushButton* m_createBookmarkButton;
	QPushButton* m_showBookmarksButton;

	QtThreadedFunctor<std::vector<std::shared_ptr<Bookmark>>> m_displayBookmarksFunctor;
	QtThreadedFunctor<std::vector<std::string>, std::vector<BookmarkCategory>> m_displayBookmarkCreatorFunctor;
	QtThreadedFunctor<std::shared_ptr<Bookmark>, std::vector<BookmarkCategory>> m_displayBookmarkEditorFunctor;
	QtThreadedFunctor<BookmarkView::CreateButtonState> m_setCreateButtonStateFunctor;
	QtThreadedFunctor<bool> m_enableDisplayButtonFunctor;

	QtBookmarkBrowser* m_bookmarkBrowser;

	BookmarkView::CreateButtonState m_createButtonState;
};

#endif // QT_BOOKMARK_BAR_H
