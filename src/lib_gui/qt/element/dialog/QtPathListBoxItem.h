#ifndef QT_PATH_LIST_BOX_ITEM_H
#define QT_PATH_LIST_BOX_ITEM_H

#include "QtListBoxItem.h"

class QPushButton;
class QListWidgetItem;

class FilePath;
class QtPathListBox;

class QtPathListBoxItem
	: public QtListBoxItem
{
	Q_OBJECT

public:
	QtPathListBoxItem(QtPathListBox* listBox, QListWidgetItem* item, QWidget *parent = nullptr);

protected:
	virtual QtListBox* getListBox() override;

private slots:
	void handleButtonPress();

private:
	virtual void onReadOnlyChanged() override;

	QtPathListBox* m_listBox;
	QPushButton* m_button;
};

#endif // QT_PATH_LIST_BOX_ITEM_H
