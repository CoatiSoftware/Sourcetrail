#include "qt/element/QtStringListBoxItem.h"

#include "qt/element/QtStringListBox.h"

QtStringListBoxItem::QtStringListBoxItem(QtStringListBox* listBox, QListWidgetItem* item, QWidget *parent)
	: QtListBoxItem(item, parent)
	, m_listBox(listBox)
{
}

QtListBox* QtStringListBoxItem::getListBox()
{
	return m_listBox;
}
