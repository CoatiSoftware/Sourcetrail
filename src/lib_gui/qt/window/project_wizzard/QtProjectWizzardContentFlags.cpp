#include "qt/window/project_wizzard/QtProjectWizzardContentFlags.h"

#include <QFormLayout>

#include "qt/element/QtDirectoryListBox.h"
#include "settings/CxxProjectSettings.h"

QtProjectWizzardContentFlags::QtProjectWizzardContentFlags(std::shared_ptr<ProjectSettings> settings, QtProjectWizzardWindow* window)
	: QtProjectWizzardContent(settings, window)
{
}

void QtProjectWizzardContentFlags::populate(QGridLayout* layout, int& row)
{
	QLabel* label = createFormLabel("Compiler Flags");
	label->setObjectName("label");
	layout->addWidget(label, row, QtProjectWizzardWindow::FRONT_COL, Qt::AlignRight);

	addHelpButton("Define compiler flags used during indexing including the dash e.g. -v", layout, row);

	m_list = new QtDirectoryListBox(this, label->text(), true);
	layout->addWidget(m_list, row, QtProjectWizzardWindow::BACK_COL);
	row++;
}

void QtProjectWizzardContentFlags::load()
{
	std::shared_ptr<CxxProjectSettings> cxxSettings = std::dynamic_pointer_cast<CxxProjectSettings>(m_settings);
	if (cxxSettings)
	{
		m_list->setStringList(cxxSettings->getCompilerFlags());
	}
}

void QtProjectWizzardContentFlags::save()
{
	std::shared_ptr<CxxProjectSettings> cxxSettings = std::dynamic_pointer_cast<CxxProjectSettings>(m_settings);
	if (cxxSettings)
	{
		cxxSettings->setCompilerFlags(m_list->getStringList());
	}
}
