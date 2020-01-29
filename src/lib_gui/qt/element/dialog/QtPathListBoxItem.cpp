#include "QtPathListBoxItem.h"

#include <QBoxLayout>
#include <QListWidget>
#include <QPushButton>

#include "QtFileDialog.h"
#include "QtIconButton.h"
#include "QtLineEdit.h"
#include "QtPathListBox.h"
#include "ResourcePaths.h"

QtPathListBoxItem::QtPathListBoxItem(QtPathListBox* listBox, QListWidgetItem* item, QWidget* parent)
	: QtListBoxItem(item, parent), m_listBox(listBox)
{
	m_button = new QtIconButton(
		ResourcePaths::getGuiPath().concatenate(L"window/dots.png"),
		ResourcePaths::getGuiPath().concatenate(L"window/dots_hover.png"));
	m_button->setIconSize(QSize(16, 16));
	m_button->setObjectName(QStringLiteral("dotsButton"));
	layout()->addWidget(m_button);

	connect(m_button, &QPushButton::clicked, this, &QtPathListBoxItem::handleButtonPress);
}

QtListBox* QtPathListBoxItem::getListBox()
{
	return m_listBox;
}

void QtPathListBoxItem::handleButtonPress()
{
	FilePath path(getText().toStdWString());
	m_listBox->makeAbsolute(path);

	QStringList list;
	switch (m_listBox->getSelectionPolicy())
	{
	case QtPathListBox::SELECTION_POLICY_FILES_ONLY:
		list.append(QtFileDialog::getOpenFileName(this, QStringLiteral("Select Directory"), path, QLatin1String("")));
		break;
	case QtPathListBox::SELECTION_POLICY_DIRECTORIES_ONLY:
		list.append(QtFileDialog::getExistingDirectory(this, QStringLiteral("Select Directory"), path));
		break;
	case QtPathListBox::SELECTION_POLICY_FILES_AND_DIRECTORIES:
		list = QtFileDialog::getFileNamesAndDirectories(this, path);
		break;
	}

	if (!list.isEmpty())
	{
		FilePath path(list.at(0).toStdWString());
		m_listBox->makeRelativeIfShorter(path);
		setText(QString::fromStdWString(path.wstr()));
	}

	for (int i = 1; i < list.size(); i++)
	{
		FilePath path(list.at(i).toStdWString());
		m_listBox->makeRelativeIfShorter(path);
		getListBox()->addListBoxItemWithText(QString::fromStdWString(path.wstr()));
	}

	selectItem();
}

void QtPathListBoxItem::onReadOnlyChanged()
{
	m_button->setEnabled(!getReadOnly());
}
