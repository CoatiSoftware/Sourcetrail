#include "qt/window/project_wizzard/QtProjectWizzardContentExtensions.h"

#include <QFormLayout>

#include "qt/element/QtDirectoryListBox.h"

QtProjectWizzardContentExtensions::QtProjectWizzardContentExtensions(
	std::shared_ptr<ProjectSettings> settings, QtProjectWizzardWindow* window
)
	: QtProjectWizzardContent(settings, window)
{
}

void QtProjectWizzardContentExtensions::populateWindow(QGridLayout* layout, int& row)
{
	layout->setRowMinimumHeight(row++, 20);

	QLabel* title = new QLabel("Source File Extensions");
	title->setWordWrap(true);
	title->setObjectName("section");
	layout->addWidget(title, row, QtProjectWizzardWindow::FRONT_COL, Qt::AlignTop);
	layout->setRowStretch(row, 0);

	QLabel* text = new QLabel("Define extensions for source files including the dot e.g. .cpp");
	text->setWordWrap(true);
	text->setOpenExternalLinks(true);
	layout->addWidget(text, row + 1, QtProjectWizzardWindow::FRONT_COL, Qt::AlignTop);
	layout->setRowStretch(row + 1, 1);

	m_sourceList = new QtDirectoryListBox(this);
	layout->addWidget(m_sourceList, row, QtProjectWizzardWindow::BACK_COL, 2, 1, Qt::AlignTop);
	row += 2;
}

void QtProjectWizzardContentExtensions::populateForm(QGridLayout* layout, int& row)
{
	QLabel* sourceLabel = createFormLabel("Source File Extensions");
	sourceLabel->setObjectName("label");
	layout->addWidget(sourceLabel, row, QtProjectWizzardWindow::FRONT_COL, Qt::AlignRight);

	addHelpButton("Define extensions for source files including the dot e.g. .cpp", layout, row);

	m_sourceList = new QtDirectoryListBox(this, true);
	layout->addWidget(m_sourceList, row, QtProjectWizzardWindow::BACK_COL);
	row++;
}

void QtProjectWizzardContentExtensions::load()
{
	m_sourceList->setStringList(m_settings->getSourceExtensions());
}

void QtProjectWizzardContentExtensions::save()
{
	m_settings->setSourceExtensions(m_sourceList->getStringList());
}
