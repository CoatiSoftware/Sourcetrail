#include "QtProjectWizardContentPathCxxPch.h"

#include "SourceGroupSettingsWithCxxPchOptions.h"
#include "utility.h"
#include "utilityFile.h"

QtProjectWizardContentPathCxxPch::QtProjectWizardContentPathCxxPch(
	std::shared_ptr<SourceGroupSettings> settings,
	std::shared_ptr<SourceGroupSettingsWithCxxPchOptions> settingsCxxPch,
	QtProjectWizardWindow* window
)
	: QtProjectWizardContentPath(window)
	, m_settings(settings)
	, m_settingsCxxPch(settingsCxxPch)
{
	setTitleString("Precompiled Header File");
	setHelpString(
		"Specify the path to the input header file that should be used to generate a precompiled header before indexing.<br />"
		"If the indexed source code is usually built using precompiled headers, using this option will speed up your indexing performance.<br />"
		"Leave blank to disable the use of precompiled headers. You can make use of environment variables with ${ENV_VAR}."
	);
	setAllowEmpty(true);
	setPlaceholderString("Not Using Precompiled Header");
}

void QtProjectWizardContentPathCxxPch::populate(QGridLayout* layout, int& row)
{
	QtProjectWizardContentPath::populate(layout, row);

	m_picker->setPickDirectory(false);
}

void QtProjectWizardContentPathCxxPch::load()
{
	m_picker->setText(QString::fromStdWString(m_settingsCxxPch->getPchInputFilePath().wstr()));
}

void QtProjectWizardContentPathCxxPch::save()
{
	m_settingsCxxPch->setPchInputFilePathFilePath(FilePath(m_picker->getText().toStdWString()));
}

std::shared_ptr<SourceGroupSettings> QtProjectWizardContentPathCxxPch::getSourceGroupSettings()
{
	return m_settings;
}
