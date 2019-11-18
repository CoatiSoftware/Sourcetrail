#include "QtStringListBox.h"

#include <QListWidget>

#include "QtStringListBoxItem.h"

QtStringListBox::QtStringListBox(QWidget* parent, const QString& listName)
	: QtListBox(parent, listName)
{
}

std::vector<std::wstring> QtStringListBox::getStrings()
{
	std::vector<std::wstring> strings;
	for (int i = 0; i < m_list->count(); ++i)
	{
		QtListBoxItem* widget = dynamic_cast<QtListBoxItem*>(m_list->itemWidget(m_list->item(i)));
		strings.push_back(widget->getText().toStdWString());
	}
	return strings;
}

void QtStringListBox::setStrings(const std::vector<std::wstring>& strings, bool readOnly)
{
	clear();

	for (const std::wstring& str: strings)
	{
		QtListBoxItem* itemWidget = addListBoxItemWithText(QString::fromStdWString(str));
		itemWidget->setReadOnly(readOnly);
	}
}

QtListBoxItem* QtStringListBox::createListBoxItem(QListWidgetItem* item)
{
	return new QtStringListBoxItem(this, item);
}
