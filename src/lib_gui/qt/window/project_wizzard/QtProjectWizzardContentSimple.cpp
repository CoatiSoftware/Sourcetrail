#include "qt/window/project_wizzard/QtProjectWizzardContentSimple.h"

#include <QCheckBox>
#include <QLabel>

#include "settings/CxxProjectSettings.h"

QtProjectWizzardContentSimple::QtProjectWizzardContentSimple(std::shared_ptr<ProjectSettings> settings, QtProjectWizzardWindow* window)
	: QtProjectWizzardContent(settings, window)
	, m_checkBox(nullptr)
{
}

void QtProjectWizzardContentSimple::populate(QGridLayout* layout, int& row)
{
	m_title = createFormLabel("Lazy Include Search");
	layout->addWidget(m_title, row, QtProjectWizzardWindow::FRONT_COL);

	m_checkBox = new QCheckBox("Search included files within the project paths");
	layout->addWidget(m_checkBox, row, QtProjectWizzardWindow::BACK_COL);

	addHelpButton(
		"If enabled Coati also uses the project paths and their subdirectories when resolving #include directives.\n\n"
		"Use this option when you know that the project is self contained but don't know which paths to "
		"specify as include paths.\n\n"
		"<b>Warning</b>: This slows down indexing speed.", layout, row);

	row++;
}

void QtProjectWizzardContentSimple::load()
{
	std::shared_ptr<CxxProjectSettings> cxxSettings = std::dynamic_pointer_cast<CxxProjectSettings>(m_settings);
	if (cxxSettings)
	{
		m_checkBox->setChecked(cxxSettings->getUseSourcePathsForHeaderSearch());
	}
}

void QtProjectWizzardContentSimple::save()
{
	std::shared_ptr<CxxProjectSettings> cxxSettings = std::dynamic_pointer_cast<CxxProjectSettings>(m_settings);
	if (cxxSettings)
	{
		cxxSettings->setUseSourcePathsForHeaderSearch(m_checkBox->isChecked());
	}
}
