#include "qt/window/project_wizzard/QtProjectWizzardContentFlags.h"

#include <QFormLayout>

#include "qt/element/QtDirectoryListBox.h"
#include "settings/SourceGroupSettingsCxx.h"

QtProjectWizzardContentFlags::QtProjectWizzardContentFlags(std::shared_ptr<SourceGroupSettings> settings, QtProjectWizzardWindow* window)
	: QtProjectWizzardContent(window)
	, m_settings(settings)
{
}

void QtProjectWizzardContentFlags::populate(QGridLayout* layout, int& row)
{
	QLabel* label = createFormLabel("Compiler Flags");
	label->setObjectName("label");
	layout->addWidget(label, row, QtProjectWizzardWindow::FRONT_COL, Qt::AlignRight);

	addHelpButton("Define compiler flags used during indexing including the dash (e.g. use \"-D RELEASE\" to add a #define for \"RELEASE\").", layout, row);

	m_list = new QtDirectoryListBox(this, label->text(), true);
	layout->addWidget(m_list, row, QtProjectWizzardWindow::BACK_COL);
	row++;
}

void QtProjectWizzardContentFlags::load()
{
	std::shared_ptr<SourceGroupSettingsCxx> cxxSettings = std::dynamic_pointer_cast<SourceGroupSettingsCxx>(m_settings);
	if (cxxSettings)
	{
		m_list->setStringList(cxxSettings->getCompilerFlags());
	}
}

void QtProjectWizzardContentFlags::save()
{
	std::shared_ptr<SourceGroupSettingsCxx> cxxSettings = std::dynamic_pointer_cast<SourceGroupSettingsCxx>(m_settings);
	if (cxxSettings)
	{
		cxxSettings->setCompilerFlags(m_list->getStringList());
	}
}
