#ifndef QT_BOOKMARK_BROWSER_H
#define QT_BOOKMARK_BROWSER_H

#include <QWidget>

#include <QComboBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QWidget>

#include <QListWidget>
#include <QTreeWidget>

#include "qt/window/QtWindow.h"

#include "utility/messaging/MessageListener.h"
#include "utility/messaging/type/MessageDeleteBookmark.h"
#include "utility/messaging/type/MessageDisplayBookmarks.h"
#include "utility/messaging/type/MessageEditBookmark.h"

class Bookmark;
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

private slots:
	void closeButtonClicked();
	void filterBoxChanged(const QString& text);
	void orderBoxChanged(const QString& text);
	void itemExpanded(QTreeWidgetItem* item);
	void itemCollapsed(QTreeWidgetItem* item);

private:
	virtual void handleMessage(MessageDeleteBookmark* message);
	virtual void handleMessage(MessageEditBookmark* message);

	MessageDisplayBookmarks::BookmarkFilter getSelectedFilter();
	MessageDisplayBookmarks::BookmarkOrder getSelectedOrder();

	QTreeWidgetItem* findOrCreateTreeCategory(const std::string& name, const Id id);

	QHBoxLayout* m_layout;
	QVBoxLayout* m_headerLayout;
	QVBoxLayout* m_bodyLayout;

	QTreeWidget* m_bookmarkTree;
	QPushButton* m_closeButton;

	QLabel* m_title;
	QLabel* m_filterLabel;
	QLabel* m_orderLabel;
	QComboBox* m_filterComboBox;
	QComboBox* m_orderComboBox;
	std::vector<std::string> m_orderNames;
	int m_currentOrderIndex;

	QWidget* m_headerBackground;
};

#endif // QT_BOOKMARK_BROWSER_H
