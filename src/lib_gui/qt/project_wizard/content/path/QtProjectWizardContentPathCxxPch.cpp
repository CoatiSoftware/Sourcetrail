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
	QtProjectWizardWindow* window)
	: QtProjectWizardContentPath(window), m_settings(settings), m_settingsCxxPch(settingsCxxPch)
{
	setTitleString(QStringLiteral("Precompiled Header File"));
	setHelpString(
		"Specify the path to the input header file that should be used to generate a precompiled "
		"header before indexing.<br />"
		"If the indexed source code is usually built using precompiled headers, using this option "
		"will speed up your indexing performance.<br />"
		"<br />"
		"If your source files use precompiled headers via \"#include &lt;pch.h&gt;\", specify "
		"\"path/to/pch.h\".<br />"
		"<br />"
		"Leave blank to disable the use of precompiled headers. You can make use of environment "
		"variables with ${ENV_VAR}.");
	setPlaceholderString(QStringLiteral("Not Using Precompiled Header"));
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
	if (std::shared_ptr<SourceGroupSettingsCxxCdb> cdbSettings =
			std::dynamic_pointer_cast<SourceGroupSettingsCxxCdb>(m_settings))
	{
		const FilePath cdbPath = cdbSettings->getCompilationDatabasePathExpandedAndAbsolute();
		std::shared_ptr<clang::tooling::JSONCompilationDatabase> cdb = utility::loadCDB(cdbPath);
		if (!cdb)
		{
			QMessageBox msgBox(m_window);
			msgBox.setText(QStringLiteral("Unable to open and read the provided compilation database file."));
			msgBox.exec();
			return false;
		}

		if (utility::containsIncludePchFlags(cdb))
		{
			if (m_settingsCxxPch->getPchInputFilePath().empty())
			{
				QMessageBox msgBox(m_window);
				msgBox.setText(
					"The provided compilation database file uses precompiled headers. If you want "
					"to make use of "
					"precompiled headers to speed up your indexer, please specify an input at "
					"Precompiled Header File.");
				QPushButton* cancelButton = msgBox.addButton(
					QStringLiteral("Cancel"), QMessageBox::ButtonRole::RejectRole);
				QPushButton* continueButton = msgBox.addButton(
					QStringLiteral("Continue"), QMessageBox::ButtonRole::AcceptRole);
				msgBox.exec();
				if (msgBox.clickedButton() == cancelButton)
				{
					return false;
				}
				return true;
			}
		}
		else
		{
			if (!m_settingsCxxPch->getPchInputFilePath().empty())
			{
				QMessageBox msgBox(m_window);
				msgBox.setText(
					"The provided compilation database file does not use precompiled headers. The "
					"specified input file at "
					"Precompiled Header File will not be used.");
				QPushButton* cancelButton = msgBox.addButton(
					QStringLiteral("Cancel"), QMessageBox::ButtonRole::RejectRole);
				QPushButton* continueButton = msgBox.addButton(
					QStringLiteral("Continue"), QMessageBox::ButtonRole::AcceptRole);
				msgBox.exec();
				if (msgBox.clickedButton() == cancelButton)
				{
					return false;
				}
				return true;
			}
		}
	}
	return true;
}

std::shared_ptr<SourceGroupSettings> QtProjectWizardContentPathCxxPch::getSourceGroupSettings()
{
	return m_settings;
}
