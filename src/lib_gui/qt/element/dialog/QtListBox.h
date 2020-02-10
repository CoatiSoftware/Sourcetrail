#ifndef QT_LIST_BOX_H
#define QT_LIST_BOX_H

#include <QFrame>
#include <QListWidget>

#include "FilePath.h"

class QHBoxLayout;
class QListWidgetItem;
class QPushButton;

class QtTextEditDialog;
class QtListBoxItem;

class QtListWidget: public QListWidget
{
	Q_OBJECT

public:
	QtListWidget(QWidget* parent = nullptr): QListWidget(parent) {}

protected:
	void mouseDoubleClickEvent(QMouseEvent* event) override;
	void wheelEvent(QWheelEvent* event) override;
};

class QtListBox: public QFrame
{
	Q_OBJECT

public:
	QtListBox(QWidget* parent, const QString& listName);
	virtual ~QtListBox() = default;

	void clear();

	QtListBoxItem* addListBoxItemWithText(const QString& text);

	void selectItem(QListWidgetItem* item);

protected:
	void addWidgetToBar(QWidget* widget);

	QtListWidget* m_list;

protected slots:
	QtListBoxItem* addListBoxItem();
	void removeListBoxItem();

	void showEditDialog();
	void canceledEditDialog();
	void savedEditDialog();

private slots:
	void doubleClicked(const QModelIndex& index);

private:
	virtual QtListBoxItem* createListBoxItem(QListWidgetItem* item) = 0;

	QHBoxLayout* m_innerBarLayout;
	QPushButton* m_addButton;
	QPushButton* m_removeButton;

	QString m_listName;

	std::shared_ptr<QtTextEditDialog> m_editDialog;
};

#endif	  // QT_LIST_BOX_H
