#include "QtProjectWizardContentPathCxxPch.h"

#include <QMessageBox>

#include "IndexerCommandCxx.h"
#include "SourceGroupSettingsCxxCdb.h"
#include "SourceGroupSettingsWithCxxPchOptions.h"
#include "utility.h"
#include "utilityFile.h"
#include "utilitySourceGroupCxx.h"

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

bool QtProjectWizardContentPathCxxPch::check()
{
	if (std::shared_ptr<SourceGroupSettingsCxxCdb> cdbSettings = std::dynamic_pointer_cast<SourceGroupSettingsCxxCdb>(m_settings))
	{
		const FilePath cdbPath = cdbSettings->getCompilationDatabasePathExpandedAndAbsolute();
		std::shared_ptr<clang::tooling::JSONCompilationDatabase> cdb = IndexerCommandCxx::loadCDB(cdbPath);
		if (!cdb)
		{
			QMessageBox msgBox;
			msgBox.setText("Unable to open and read the provided compilation database file.");
			msgBox.exec();
			return false;
		}

		if (utility::containsIncludePchFlags(cdb))
		{
			if (m_settingsCxxPch->getPchInputFilePath().empty())
			{
				QMessageBox msgBox;
				msgBox.setText("The provided compilation database file uses precompiled headers. Please specify a Precompiled Header input file.");
				msgBox.exec();
				return false;
			}
		}
		else
		{
			if (!m_settingsCxxPch->getPchInputFilePath().empty())
			{
				QMessageBox msgBox;
				msgBox.setText("The provided compilation database file does not use precompiled headers. Please do not specify a Precompiled Header input file.");
				msgBox.exec();
				return false;
			}
		}
	}
	return true;
}

std::shared_ptr<SourceGroupSettings> QtProjectWizardContentPathCxxPch::getSourceGroupSettings()
{
	return m_settings;
}
