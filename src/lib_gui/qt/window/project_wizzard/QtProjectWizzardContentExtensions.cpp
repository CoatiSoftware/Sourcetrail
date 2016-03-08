#include "qt/window/project_wizzard/QtProjectWizzardContentExtensions.h"

#include <QFormLayout>

#include "qt/element/QtDirectoryListBox.h"

QtProjectWizzardContentExtensions::QtProjectWizzardContentExtensions(
	ProjectSettings* settings, QtProjectWizzardWindow* window
)
	: QtProjectWizzardContent(settings, window)
{
}

void QtProjectWizzardContentExtensions::populateForm(QGridLayout* layout, int& row)
{
	QLabel* headerLabel = createFormLabel("Header File Extensions");
	headerLabel->setObjectName("label");
	layout->addWidget(headerLabel, row, QtProjectWizzardWindow::FRONT_COL, Qt::AlignRight);

	addHelpButton("Define extensions for header files including the dot e.g. .h", layout, row);

	m_headerList = new QtDirectoryListBox(this, true);
	layout->addWidget(m_headerList, row, QtProjectWizzardWindow::BACK_COL);
	row++;

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
	m_headerList->setStringList(m_settings->getHeaderExtensions());
	m_sourceList->setStringList(m_settings->getSourceExtensions());
}

void QtProjectWizzardContentExtensions::save()
{
	m_settings->setHeaderExtensions(m_headerList->getStringList());
	m_settings->setSourceExtensions(m_sourceList->getStringList());
}
