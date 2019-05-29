#include "QtProjectWizardContentPathDependenciesGradle.h"

#include "SourceGroupSettingsJavaGradle.h"

QtProjectWizardContentPathDependenciesGradle::QtProjectWizardContentPathDependenciesGradle(
	std::shared_ptr<SourceGroupSettingsJavaGradle> settings, QtProjectWizardWindow* window
)
	: QtProjectWizardContentPath(window)
	, m_settings(settings)
{
	setTitleString("Intermediate Dependencies Directory");
	setHelpString(
		"This directory is used to temporarily download and store the dependencies (e.g. .jar files) of the Gradle "
		"project while it is indexed.<br />"
		"<br />"
		"You can make use of environment variables with ${ENV_VAR}."
	);
}

void QtProjectWizardContentPathDependenciesGradle::load()
{
	m_picker->setText(QString::fromStdWString(m_settings->getGradleDependenciesDirectory().wstr()));
}

void QtProjectWizardContentPathDependenciesGradle::save()
{
	m_settings->setGradleDependenciesDirectory(FilePath(m_picker->getText().toStdWString()));
}

std::shared_ptr<SourceGroupSettings> QtProjectWizardContentPathDependenciesGradle::getSourceGroupSettings()
{
	return m_settings;
}
