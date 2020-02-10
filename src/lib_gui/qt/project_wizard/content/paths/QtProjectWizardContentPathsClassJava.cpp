#include "QtProjectWizardContentPathsClassJava.h"

#include <QCheckBox>

#include "SourceGroupSettings.h"
#include "SourceGroupSettingsWithClasspath.h"

QtProjectWizardContentPathsClassJava::QtProjectWizardContentPathsClassJava(
	std::shared_ptr<SourceGroupSettings> settings, QtProjectWizardWindow* window)
	: QtProjectWizardContentPaths(
		  settings, window, QtPathListBox::SELECTION_POLICY_FILES_AND_DIRECTORIES, true)
{
	setTitleString("Class Path");
	setHelpString(
		"Enter all the .jar files your project depends on. If your project depends on uncompiled "
		"java code that should "
		"not be indexed, please add the root directory of those .java files here (the one where "
		"all the package names "
		"are relative to).<br />"
		"<br />"
		"You can make use of environment variables with ${ENV_VAR}.");
}

void QtProjectWizardContentPathsClassJava::populate(QGridLayout* layout, int& row)
{
	QtProjectWizardContentPaths::populate(layout, row);

	QLabel* label = createFormLabel("JRE System Library");
	layout->addWidget(label, row, QtProjectWizardWindow::FRONT_COL, Qt::AlignTop);

	m_useJreSystemLibraryCheckBox = new QCheckBox("Use JRE System Library", this);

	layout->addWidget(m_useJreSystemLibraryCheckBox, row, QtProjectWizardWindow::BACK_COL);
	row++;
}

void QtProjectWizardContentPathsClassJava::load()
{
	std::shared_ptr<SourceGroupSettingsWithClasspath> settings =
		std::dynamic_pointer_cast<SourceGroupSettingsWithClasspath>(m_settings);
	if (settings)
	{
		m_list->setPaths(settings->getClasspath());
		m_useJreSystemLibraryCheckBox->setChecked(settings->getUseJreSystemLibrary());
	}
}

void QtProjectWizardContentPathsClassJava::save()
{
	std::shared_ptr<SourceGroupSettingsWithClasspath> settings =
		std::dynamic_pointer_cast<SourceGroupSettingsWithClasspath>(m_settings);
	if (settings)
	{
		settings->setClasspath(m_list->getPathsAsDisplayed());
		settings->setUseJreSystemLibrary(m_useJreSystemLibraryCheckBox->isChecked());
	}
}
