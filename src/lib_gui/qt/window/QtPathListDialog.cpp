#include "qt/window/QtPathListDialog.h"

#include <QLabel>
#include "qt/element/QtDirectoryListBox.h"

QtPathListDialog::QtPathListDialog(const QString& title, const QString& description, QWidget* parent)
	: QtWindow(false, parent)
	, m_title(title)
	, m_description(description)
{
}

QSize QtPathListDialog::sizeHint() const
{
	return QSize(550, 350);
}

void QtPathListDialog::setRelativeRootDirectory(const FilePath& dir)
{
	m_pathList->setRelativeRootDirectory(dir);
}

void QtPathListDialog::setPaths(const std::vector<FilePath>& paths, bool readOnly)
{
	m_pathList->setList(paths, readOnly);
}

std::vector<FilePath> QtPathListDialog::getPaths()
{
	return m_pathList->getList();
}

void QtPathListDialog::populateWindow(QWidget* widget)
{
	QVBoxLayout* layout = new QVBoxLayout();
	layout->setContentsMargins(0, 0, 0, 0);

	QLabel* description = new QLabel(m_description);
	description->setObjectName("description");
	description->setWordWrap(true);
	layout->addWidget(description);

	layout->addSpacing(10);

	m_pathList = new QtDirectoryListBox(nullptr, m_title);
	m_pathList->setMaximumHeight(1000);
	layout->addWidget(m_pathList);

	widget->setLayout(layout);
}

void QtPathListDialog::windowReady()
{
	updateNextButton("Save");
	updateCloseButton("Cancel");

	setPreviousVisible(false);

	updateTitle(m_title);
}
