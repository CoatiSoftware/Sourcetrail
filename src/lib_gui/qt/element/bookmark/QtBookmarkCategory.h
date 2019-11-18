#ifndef QT_BOOKMARK_CATEGORY_H
#define QT_BOOKMARK_CATEGORY_H

#include <QFrame>

#include "BookmarkController.h"
#include "ControllerProxy.h"
#include "types.h"

class QLabel;
class QPushButton;
class QTreeWidgetItem;

class QtBookmarkCategory: public QFrame
{
	Q_OBJECT

public:
	QtBookmarkCategory(ControllerProxy<BookmarkController>* controllerProxy);
	~QtBookmarkCategory();

	void setName(const std::wstring& name);
	std::wstring getName() const;

	void setId(const Id id);
	Id getId() const;

	void setTreeWidgetItem(QTreeWidgetItem* treeItem);

	void updateArrow();

public slots:
	void expandClicked();

protected:
	virtual void enterEvent(QEvent* event);
	virtual void leaveEvent(QEvent* event);

private slots:
	void deleteClicked();

private:
	ControllerProxy<BookmarkController>* m_controllerProxy;

	QLabel* m_name;
	Id m_id;

	QPushButton* m_expandButton;
	QPushButton* m_deleteButton;

	QTreeWidgetItem* m_treeItem;	// store a pointer to the 'parent' tree item to enable the
									// custom expand button
};

#endif	  // QT_BOOKMARK_CATEGORY_H
