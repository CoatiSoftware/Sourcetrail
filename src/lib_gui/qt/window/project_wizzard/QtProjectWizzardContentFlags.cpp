#include "qt/window/project_wizzard/QtProjectWizzardContentFlags.h"

#include <QFormLayout>

#include "qt/element/QtStringListBox.h"
#include "settings/SourceGroupSettingsCxx.h"

QtProjectWizzardContentFlags::QtProjectWizzardContentFlags(std::shared_ptr<SourceGroupSettings> settings, QtProjectWizzardWindow* window, bool isCDB)
	: QtProjectWizzardContent(window)
	, m_settings(settings)
	, m_isCdb(isCDB)
{
}

void QtProjectWizzardContentFlags::populate(QGridLayout* layout, int& row)
{
	QString labelText((std::string(m_isCdb ? "Additional " : "") + "Compiler Flags").c_str());
	QLabel* label = createFormLabel(labelText);
	layout->addWidget(label, row, QtProjectWizzardWindow::FRONT_COL, Qt::AlignTop);

	addHelpButton(labelText, "Define additional Clang compiler flags used during indexing including the dash (e.g. use \"-D RELEASE\" to add a #define for \"RELEASE\").", layout, row);

	m_list = new QtStringListBox(this, label->text());
	layout->addWidget(m_list, row, QtProjectWizzardWindow::BACK_COL);
	row++;
}

void QtProjectWizzardContentFlags::load()
{
	std::shared_ptr<SourceGroupSettingsCxx> cxxSettings = std::dynamic_pointer_cast<SourceGroupSettingsCxx>(m_settings);
	if (cxxSettings)
	{
		m_list->setStrings(cxxSettings->getCompilerFlags());
	}
}

void QtProjectWizzardContentFlags::save()
{
	std::shared_ptr<SourceGroupSettingsCxx> cxxSettings = std::dynamic_pointer_cast<SourceGroupSettingsCxx>(m_settings);
	if (cxxSettings)
	{
		cxxSettings->setCompilerFlags(m_list->getStrings());
	}
}
