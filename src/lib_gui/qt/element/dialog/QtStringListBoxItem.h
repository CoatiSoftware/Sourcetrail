#ifndef QT_STRING_LIST_BOX_ITEM_H
#define QT_STRING_LIST_BOX_ITEM_H

#include "QtListBoxItem.h"

class QListWidgetItem;

class QtListBox;
class QtStringListBox;

class QtStringListBoxItem: public QtListBoxItem
{
	Q_OBJECT

public:
	QtStringListBoxItem(QtStringListBox* listBox, QListWidgetItem* item, QWidget* parent = nullptr);

protected:
	virtual QtListBox* getListBox() override;

private:
	QtStringListBox* m_listBox;
};

#endif	  // QT_STRING_LIST_BOX_ITEM_H
