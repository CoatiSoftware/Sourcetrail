#include "qt/window/QtSelectPathsDialog.h"

#include <QLabel>
#include <QListWidget>

QtSelectPathsDialog::QtSelectPathsDialog(const QString& title, const QString& description, QWidget* parent)
	: QtTextEditDialog(title, description, parent)
{
}

std::vector<FilePath> QtSelectPathsDialog::getPathsList() const
{
	std::vector<FilePath> checkedPaths;

	for (int i = 0; i < m_list->count(); i++)
	{
		if (m_list->item(i)->checkState() ==  Qt::Checked)
		{
			checkedPaths.push_back(m_list->item(i)->text().toStdString());
		}
	}

	return checkedPaths;
}

void QtSelectPathsDialog::setPathsList(const std::vector<FilePath>& paths, const std::vector<FilePath>& checkedPaths)
{
	for (FilePath s : paths)
	{
		QListWidgetItem* item = new QListWidgetItem(s.str().c_str(), m_list);
		item->setFlags(item->flags() | Qt::ItemIsUserCheckable); // set checkable flag

		if (std::find(checkedPaths.begin(), checkedPaths.end(), s) == checkedPaths.end())
		{
			item->setCheckState(Qt::Unchecked); // AND initialize check state
		}
		else
		{
			item->setCheckState(Qt::Checked);
		}

		if (!s.exists())
		{
			item->setTextColor(Qt::red);
			item->setToolTip("Path does not exist");
			item->setFlags( item->flags() & ~Qt::ItemIsEnabled );
		}
		else
		{
			item->setTextColor(Qt::black);
		}
	}
}

void QtSelectPathsDialog::populateWindow(QWidget* widget)
{
	QVBoxLayout* layout = new QVBoxLayout();
	layout->setContentsMargins(0, 0, 0, 0);

	QLabel* description = new QLabel(m_description);
	description->setObjectName("description");
	description->setWordWrap(true);
	layout->addWidget(description);

	m_list = new QListWidget();
	m_list->setObjectName("pathList");
	m_list->setEditTriggers(QAbstractItemView::NoEditTriggers);
	m_list->setSelectionMode(QAbstractItemView::NoSelection);
	m_list->setAttribute(Qt::WA_MacShowFocusRect, 0);
	layout->addWidget(m_list);

	widget->setLayout(layout);
}

void QtSelectPathsDialog::windowReady()
{
	updateNextButton("Save");
	updateCloseButton("Cancel");

	setPreviousVisible(false);

	updateTitle(m_title);
}
