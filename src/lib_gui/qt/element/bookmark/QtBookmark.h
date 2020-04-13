#ifndef QT_BOOKMARK_H
#define QT_BOOKMARK_H

#include <QFrame>
#include <QLabel>
#include <QPushButton>
#include <QTreeWidget>

#include "../../../../lib/data/bookmark/Bookmark.h"
#include "../../../../lib/component/controller/BookmarkController.h"
#include "../../../../lib/component/controller/helper/ControllerProxy.h"

class Bookmark;

class QtBookmark: public QFrame
{
	Q_OBJECT

public:
	QtBookmark(ControllerProxy<BookmarkController>* controllerProxy);
	virtual ~QtBookmark();

	void setBookmark(const std::shared_ptr<Bookmark> bookmark);
	Id getBookmarkId() const;

	QTreeWidgetItem* getTreeWidgetItem() const;
	void setTreeWidgetItem(QTreeWidgetItem* treeWidgetItem);

public slots:
	void commentToggled();

protected:
	virtual void resizeEvent(QResizeEvent* event) override;
	virtual void showEvent(QShowEvent* event) override;

	virtual void enterEvent(QEvent* event) override;
	virtual void leaveEvent(QEvent* event) override;

private slots:
	void activateClicked();
	void editClicked();
	void deleteClicked();
	void elideButtonText();

private:
	void updateArrow();

	std::string getDateString() const;

	ControllerProxy<BookmarkController>* m_controllerProxy;

	QPushButton* m_activateButton;
	QPushButton* m_editButton;
	QPushButton* m_deleteButton;
	QPushButton* m_toggleCommentButton;

	QLabel* m_comment;
	QLabel* m_dateLabel;

	std::shared_ptr<Bookmark> m_bookmark;

	// pointer to the bookmark category item in the treeView, allows to refresh tree view when a
	// node changes in size (e.g. toggle comment). Not a nice solution to the problem, but couldn't
	// find anything better yet. (sizeHintChanged signal can't be emitted here...)
	QTreeWidgetItem* m_treeWidgetItem;

	std::wstring m_arrowImageName;
	bool m_hovered;

	bool m_ignoreNextResize;
};

#endif	  // QT_BOOKMARK_H
