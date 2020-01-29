#include "QtPathListDialog.h"

#include <QLabel>

QtPathListDialog::QtPathListDialog(
	const QString& title,
	const QString& description,
	QtPathListBox::SelectionPolicyType selectionPolicy,
	QWidget* parent)
	: QtWindow(false, parent)
	, m_title(title)
	, m_description(description)
	, m_selectionPolicy(selectionPolicy)
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
	m_pathList->setPaths(paths, readOnly);
}

std::vector<FilePath> QtPathListDialog::getPaths()
{
	return m_pathList->getPathsAsDisplayed();
}

void QtPathListDialog::populateWindow(QWidget* widget)
{
	QVBoxLayout* layout = new QVBoxLayout();
	layout->setContentsMargins(0, 0, 0, 0);

	QLabel* description = new QLabel(m_description);
	description->setObjectName(QStringLiteral("description"));
	description->setWordWrap(true);
	layout->addWidget(description);

	layout->addSpacing(10);

	m_pathList = new QtPathListBox(nullptr, m_title, m_selectionPolicy);
	m_pathList->setMaximumHeight(1000);
	layout->addWidget(m_pathList);

	widget->setLayout(layout);
}

void QtPathListDialog::windowReady()
{
	updateNextButton(QStringLiteral("Save"));
	updateCloseButton(QStringLiteral("Cancel"));

	setPreviousVisible(false);

	updateTitle(m_title);
}
