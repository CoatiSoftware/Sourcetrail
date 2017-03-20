#ifndef QT_BOOKMARK_BROWSER_H
#define QT_BOOKMARK_BROWSER_H

#include <QComboBox>
#include <QListWidget>
#include <QTreeWidget>

#include "qt/window/QtWindow.h"

#include "utility/messaging/MessageListener.h"
#include "utility/messaging/type/MessageDeleteBookmark.h"
#include "utility/messaging/type/MessageDisplayBookmarks.h"
#include "utility/messaging/type/MessageEditBookmark.h"

class Bookmark;
class BookmarkCategory;
class QtBookmark;

class QtBookmarkBrowser
	: public QtWindow
	, public MessageListener<MessageDeleteBookmark>
	, public MessageListener<MessageEditBookmark>
{
	Q_OBJECT

public:
	QtBookmarkBrowser(QWidget* parent = nullptr);
	~QtBookmarkBrowser();

	void setupBookmarkBrowser();
	void setBookmarks(const std::vector<std::shared_ptr<Bookmark>>& bookmarks);

protected:
	virtual void resizeEvent(QResizeEvent* event) Q_DECL_OVERRIDE;

	virtual void handleClose() override;
	virtual void handleNext() override;

private slots:
	void filterOrOrderChanged(const QString& text);
	void categoryExpansionChanged(QTreeWidgetItem* item);
	void treeItemClicked(QTreeWidgetItem* item, int column);

private:
	virtual void handleMessage(MessageDeleteBookmark* message) override;
	virtual void handleMessage(MessageEditBookmark* message) override;

	MessageDisplayBookmarks::BookmarkFilter getSelectedFilter();
	MessageDisplayBookmarks::BookmarkOrder getSelectedOrder();

	QTreeWidgetItem* findOrCreateTreeCategory(const BookmarkCategory& category);

	QTreeWidget* m_bookmarkTree;

	QComboBox* m_filterComboBox;
	QComboBox* m_orderComboBox;
	std::vector<std::string> m_orderNames;

	QWidget* m_headerBackground;
};

#endif // QT_BOOKMARK_BROWSER_H
