#ifndef QT_BOOKMARK_H
#define QT_BOOKMARK_H

#include <QFrame>

#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>

#include "utility/messaging/MessageListener.h"
#include "utility/messaging/type/MessageDeleteBookmark.h"
#include "utility/messaging/type/MessageEditBookmark.h"

#include "data/bookmark/Bookmark.h"

#include <QTreeWidget>

class QtBookmark
	: public QFrame
	, public MessageListener<MessageEditBookmark>
{
	Q_OBJECT

public:
	QtBookmark();
	virtual ~QtBookmark();

	void setName(const std::string& name);
	std::string getName() const;

	void setBookmark(const std::shared_ptr<Bookmark> bookmark);

	Id getBookmarkId() const;

	QTreeWidgetItem* getTreeWidgetItem() const;
	void setTreeWidgetItem(QTreeWidgetItem* treeWidgetItem);

	void refreshStyle();

private slots:
	void activateClicked();
	void editClicked();
	void deleteClicked();
	void commentToggled();

private:
	void handleMessage(MessageEditBookmark* message);

	std::string getDateString() const;

	QVBoxLayout* m_layout;

	QPushButton* m_activateButton;
	QPushButton* m_editButton;
	QPushButton* m_deleteButton;
	QPushButton* m_toggleCommentButton;

	QLabel* m_comment;
	QLabel* m_dateLabel;

	std::shared_ptr<Bookmark> m_bookmark;

	QTreeWidgetItem* m_treeWidgetItem; // pointer to the bookmark category item in the treeView, allows to refresh tree view when a node changes in size (e.g. toggle comment)
									// not a nice solution to the problem, but couldn't find anything better yet (sizeHintChanged signal can't be emitted here...)
};

#endif // QT_BOOKMARK_H