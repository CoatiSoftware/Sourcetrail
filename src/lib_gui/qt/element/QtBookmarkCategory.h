#ifndef QT_BOOKMARK_CATEGORY_H
#define QT_BOOKMARK_CATEGORY_H

#include <QFrame>
#include <QLabel>
#include <QPushButton>
#include <QTreeWidget>

#include "utility/types.h"

class QtBookmarkCategory
	: public QFrame
{
	Q_OBJECT

public:
	QtBookmarkCategory();
	~QtBookmarkCategory();

	void setName(const std::string& name);
	std::string getName() const;

	void setId(const Id id);
	Id getId() const;

	void setTreeWidgetItem(QTreeWidgetItem* treeItem);

	void updateArrow();

public slots:
	void expandClicked();

protected:
	virtual void enterEvent(QEvent *event);
	virtual void leaveEvent(QEvent *event);

private slots:
	void deleteClicked();

private:
	QLabel* m_name;
	Id m_id;

	QPushButton* m_expandButton;
	QPushButton* m_deleteButton;

	QTreeWidgetItem* m_treeItem; // store a pointer to the 'parent' tree item to enable the custom expand button
};

#endif // QT_BOOKMARK_CATEGORY_H
