#include "QtStringListBoxItem.h"

#include "QtStringListBox.h"

QtStringListBoxItem::QtStringListBoxItem(QtStringListBox* listBox, QListWidgetItem* item, QWidget *parent)
	: QtListBoxItem(item, parent)
	, m_listBox(listBox)
{
}

QtListBox* QtStringListBoxItem::getListBox()
{
	return m_listBox;
}
