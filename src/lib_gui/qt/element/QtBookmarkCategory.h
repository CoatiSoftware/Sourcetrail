#ifndef QT_BOOKMARK_CATEGORY_H
#define QT_BOOKMARK_CATEGORY_H

#include <QFrame>

#include <QLabel>
#include <QHBoxLayout>
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

private slots:
	void expandClicked();
	void deleteClicked();

private:
	QHBoxLayout* m_layout;
	QLabel* m_name;

	QPushButton* m_expandButton;
	QPushButton* m_deleteButton;

	QTreeWidgetItem* m_treeItem; // store a pointer to the 'parent' tree item to enable the custom expand button

	Id m_id;
};

#endif // QT_BOOKMARK_CATEGORY_H