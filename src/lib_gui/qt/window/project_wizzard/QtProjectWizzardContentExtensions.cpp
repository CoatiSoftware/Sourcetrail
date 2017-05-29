#include "qt/window/project_wizzard/QtProjectWizzardContentExtensions.h"

#include <QFormLayout>

#include "settings/SourceGroupSettings.h"
#include "qt/element/QtDirectoryListBox.h"

QtProjectWizzardContentExtensions::QtProjectWizzardContentExtensions(
	std::shared_ptr<SourceGroupSettings> settings, QtProjectWizzardWindow* window
)
	: QtProjectWizzardContent(window)
	, m_settings(settings)
{
}

void QtProjectWizzardContentExtensions::populate(QGridLayout* layout, int& row)
{
	QLabel* sourceLabel = createFormLabel("Source File Extensions");
	layout->addWidget(sourceLabel, row, QtProjectWizzardWindow::FRONT_COL, Qt::AlignTop);

	addHelpButton("Define extensions for source files including the dot e.g. .cpp", layout, row);

	m_sourceList = new QtDirectoryListBox(this, sourceLabel->text(), true);
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
