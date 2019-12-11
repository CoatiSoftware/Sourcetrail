#include "QtProjectWizardContentPathSettingsMaven.h"

#include <QCheckBox>

//#include "Application.h"
//#include "ApplicationSettings.h"
//#include "MessageStatus.h"
//#include "QtDialogView.h"
//#include "ScopedFunctor.h"
//#include "SourceGroupJavaMaven.h"
#include "SourceGroupSettingsJavaMaven.h"
//#include "logging.h"
//#include "utility.h"
//#include "utilityFile.h"
//#include "utilityMaven.h"

QtProjectWizardContentPathSettingsMaven::QtProjectWizardContentPathSettingsMaven(
	std::shared_ptr<SourceGroupSettingsJavaMaven> settings, QtProjectWizardWindow* window)
	: QtProjectWizardContentPath(window), m_settings(settings)
{
	setTitleString("Maven Settings File (settings.xml)");
	setHelpString(
		"If your project uses a custom Maven settings file, specify it here. "
		"If you leave this option empty, the default Maven settings will be used.<br />"
		"<br />"
		"You can make use of environment variables with ${ENV_VAR}.");
	setPlaceholderString("Use Default");
	setAllowEmpty(true);
	setFileEndings({L".xml"});
}

void QtProjectWizardContentPathSettingsMaven::populate(QGridLayout* layout, int& row)
{
	QtProjectWizardContentPath::populate(layout, row);
	m_picker->setPickDirectory(false);
	m_picker->setFileFilter("Settings File (*.xml)");
}

void QtProjectWizardContentPathSettingsMaven::load()
{
	m_picker->setText(QString::fromStdWString(m_settings->getMavenSettingsFilePath().wstr()));
}

void QtProjectWizardContentPathSettingsMaven::save()
{
	m_settings->setMavenSettingsFilePath(FilePath(m_picker->getText().toStdWString()));
}

std::shared_ptr<SourceGroupSettings> QtProjectWizardContentPathSettingsMaven::getSourceGroupSettings()
{
	return m_settings;
}
