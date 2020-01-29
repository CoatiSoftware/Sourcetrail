#include "QtSelectPathsDialog.h"

#include <set>

#include <QLabel>
#include <QListWidget>
#include <QPushButton>

#include "FilePath.h"
#include "utility.h"

QtSelectPathsDialog::QtSelectPathsDialog(const QString& title, const QString& description, QWidget* parent)
	: QtTextEditDialog(title, description, parent)
{
}

std::vector<FilePath> QtSelectPathsDialog::getPathsList() const
{
	std::vector<FilePath> checkedPaths;

	for (int i = 0; i < m_list->count(); i++)
	{
		if (m_list->item(i)->checkState() == Qt::Checked)
		{
			checkedPaths.push_back(FilePath(m_list->item(i)->text().toStdWString()));
		}
	}

	return checkedPaths;
}

void QtSelectPathsDialog::setPathsList(
	const std::vector<FilePath>& paths,
	const std::vector<FilePath>& checkedPaths,
	const FilePath& rootPathForRelativePaths)
{
	std::set<FilePath> checked(checkedPaths.begin(), checkedPaths.end());

	for (FilePath s: utility::unique(utility::concat(paths, checkedPaths)))
	{
		QListWidgetItem* item = new QListWidgetItem(QString::fromStdWString(s.wstr()), m_list);
		item->setFlags(item->flags() | Qt::ItemIsUserCheckable);	// set checkable flag

		if (checked.find(s) == checked.end())
		{
			item->setCheckState(Qt::Unchecked);	   // AND initialize check state
		}
		else
		{
			item->setCheckState(Qt::Checked);
		}

		if (!s.isAbsolute())
		{
			s = rootPathForRelativePaths.getConcatenated(s);
		}

		if (!s.exists())
		{
			item->setTextColor(Qt::red);
			item->setToolTip(QStringLiteral("Path does not exist"));
			item->setFlags(item->flags() & ~Qt::ItemIsEnabled);
			item->setCheckState(Qt::Unchecked);
		}
		else
		{
			item->setTextColor(Qt::black);
		}
	}
}

void QtSelectPathsDialog::checkSelected(bool checked)
{
	for (QListWidgetItem* item: m_list->selectedItems())
	{
		item->setCheckState((checked ? Qt::Checked : Qt::Unchecked));
	}
}

void QtSelectPathsDialog::populateWindow(QWidget* widget)
{
	QVBoxLayout* layout = new QVBoxLayout();
	layout->setContentsMargins(0, 0, 0, 0);

	QLabel* description = new QLabel(m_description);
	description->setObjectName(QStringLiteral("description"));
	description->setWordWrap(true);
	layout->addWidget(description);

	m_list = new QListWidget();
	m_list->setObjectName(QStringLiteral("pathList"));
	m_list->setEditTriggers(QAbstractItemView::NoEditTriggers);
	m_list->setSelectionMode(QAbstractItemView::ExtendedSelection);
	m_list->setAttribute(Qt::WA_MacShowFocusRect, 0);
	layout->addWidget(m_list);

	QHBoxLayout* buttonLayout = new QHBoxLayout();
	buttonLayout->setContentsMargins(0, 0, 0, 0);

	QPushButton* checkAllButton = new QPushButton(QStringLiteral("check all"));
	checkAllButton->setObjectName(QStringLiteral("windowButton"));
	connect(checkAllButton, &QPushButton::clicked, [=]() {
		m_list->selectAll();
		checkSelected(true);
		m_list->clearSelection();
	});
	buttonLayout->addWidget(checkAllButton);

	QPushButton* unCheckAllButton = new QPushButton(QStringLiteral("uncheck all"));
	unCheckAllButton->setObjectName(QStringLiteral("windowButton"));
	connect(unCheckAllButton, &QPushButton::clicked, [=]() {
		m_list->selectAll();
		checkSelected(false);
		m_list->clearSelection();
	});
	buttonLayout->addWidget(unCheckAllButton);

	QPushButton* checkSelectedButton = new QPushButton(QStringLiteral("check selected"));
	checkSelectedButton->setObjectName(QStringLiteral("windowButton"));
	connect(checkSelectedButton, &QPushButton::clicked, [=]() { checkSelected(true); });
	buttonLayout->addWidget(checkSelectedButton);

	QPushButton* unCheckSelectedButton = new QPushButton(QStringLiteral("uncheck selected"));
	unCheckSelectedButton->setObjectName(QStringLiteral("windowButton"));
	connect(unCheckSelectedButton, &QPushButton::clicked, [=]() { checkSelected(false); });
	buttonLayout->addWidget(unCheckSelectedButton);

	layout->addLayout(buttonLayout);

	widget->setLayout(layout);
}

void QtSelectPathsDialog::windowReady()
{
	updateNextButton(QStringLiteral("Save"));
	updateCloseButton(QStringLiteral("Cancel"));

	setPreviousVisible(false);

	updateTitle(m_title);
}
