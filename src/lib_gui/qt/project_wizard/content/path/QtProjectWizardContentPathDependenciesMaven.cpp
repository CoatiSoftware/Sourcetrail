#include "QtProjectWizardContentPathDependenciesMaven.h"

#include "SourceGroupSettingsJavaMaven.h"

QtProjectWizardContentPathDependenciesMaven::QtProjectWizardContentPathDependenciesMaven(
	std::shared_ptr<SourceGroupSettingsJavaMaven> settings, QtProjectWizardWindow* window
)
	: QtProjectWizardContentPath(window)
	, m_settings(settings)
{
	setTitleString("Intermediate Dependencies Directory");
	setHelpString(
		"This directory is used to temporarily download and store the dependencies (e.g. .jar files) of the Maven project while it is indexed.<br />"
		"<br />"
		"You can make use of environment variables with ${ENV_VAR}."
	);
}

void QtProjectWizardContentPathDependenciesMaven::load()
{
	m_picker->setText(QString::fromStdWString(m_settings->getMavenDependenciesDirectory().wstr()));
}

void QtProjectWizardContentPathDependenciesMaven::save()
{
	m_settings->setMavenDependenciesDirectory(FilePath(m_picker->getText().toStdWString()));
}

std::shared_ptr<SourceGroupSettings> QtProjectWizardContentPathDependenciesMaven::getSourceGroupSettings()
{
	return m_settings;
}
