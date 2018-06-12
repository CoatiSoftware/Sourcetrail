#include "qt/window/project_wizzard/QtProjectWizzardContentExtensions.h"

#include <QFormLayout>

#include "settings/SourceGroupSettingsWithSourceExtensions.h"
#include "qt/element/QtStringListBox.h"

QtProjectWizzardContentExtensions::QtProjectWizzardContentExtensions(
	std::shared_ptr<SourceGroupSettingsWithSourceExtensions> settings, 
	QtProjectWizzardWindow* window
)
	: QtProjectWizzardContent(window)
	, m_settings(settings)
{
}

void QtProjectWizzardContentExtensions::populate(QGridLayout* layout, int& row)
{
	QLabel* sourceLabel = createFormLabel("Source File Extensions");
	layout->addWidget(sourceLabel, row, QtProjectWizzardWindow::FRONT_COL, Qt::AlignTop);

	addHelpButton("Source File Extensions", "Define extensions for source files including the dot (e.g. .cpp or .java)", layout, row);

	m_listBox = new QtStringListBox(this, sourceLabel->text());
	layout->addWidget(m_listBox, row, QtProjectWizzardWindow::BACK_COL);
	row++;
}

void QtProjectWizzardContentExtensions::load()
{
	m_listBox->setStrings(m_settings->getSourceExtensions());
}

void QtProjectWizzardContentExtensions::save()
{
	m_settings->setSourceExtensions(m_listBox->getStrings());
}
