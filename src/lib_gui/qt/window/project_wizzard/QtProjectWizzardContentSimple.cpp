#include "qt/window/project_wizzard/QtProjectWizzardContentSimple.h"

#include <QCheckBox>
#include <QLabel>

#include "settings/SourceGroupSettingsCxx.h"

QtProjectWizzardContentSimple::QtProjectWizzardContentSimple(std::shared_ptr<SourceGroupSettings> settings, QtProjectWizzardWindow* window)
	: QtProjectWizzardContent(window)
	, m_settings(settings)
	, m_checkBox(nullptr)
{
}

void QtProjectWizzardContentSimple::populate(QGridLayout* layout, int& row)
{
	m_title = createFormLabel("Lazy Include Search (deprecated)");
	layout->addWidget(m_title, row, QtProjectWizzardWindow::FRONT_COL);

	m_checkBox = new QCheckBox("Search included files within the project paths");
	layout->addWidget(m_checkBox, row, QtProjectWizzardWindow::BACK_COL);

	addHelpButton(
		"If enabled Sourcetrail also uses the project paths and their subdirectories when resolving #include directives.<br />"
		"<br />"
		"Use this option when you know that the project is self contained but don't know which paths to "
		"specify as include paths.<br />"
		"<br />"
		"<b>Warning</b>: This slows down indexing speed.<br />"
		"<br />"
		"<b>Deprecated</b>: This option will be removed in future versions.", layout, row);

	row++;
}

void QtProjectWizzardContentSimple::load()
{
	std::shared_ptr<SourceGroupSettingsCxx> cxxSettings = std::dynamic_pointer_cast<SourceGroupSettingsCxx>(m_settings);
	if (cxxSettings)
	{
		m_checkBox->setChecked(cxxSettings->getUseSourcePathsForHeaderSearch());
	}
}

void QtProjectWizzardContentSimple::save()
{
	std::shared_ptr<SourceGroupSettingsCxx> cxxSettings = std::dynamic_pointer_cast<SourceGroupSettingsCxx>(m_settings);
	if (cxxSettings)
	{
		cxxSettings->setUseSourcePathsForHeaderSearch(m_checkBox->isChecked());
	}
}
