#ifndef QT_PROJECT_WIZARD_CONTENT_PATH_DEPENDENCIES_MAVEN_H
#define QT_PROJECT_WIZARD_CONTENT_PATH_DEPENDENCIES_MAVEN_H

#include "QtProjectWizardContentPath.h"

class SourceGroupSettingsJavaMaven;

class QtProjectWizardContentPathDependenciesMaven
	: public QtProjectWizardContentPath
{
	Q_OBJECT
public:
	QtProjectWizardContentPathDependenciesMaven(
		std::shared_ptr<SourceGroupSettingsJavaMaven> settings, QtProjectWizardWindow* window);

	// QtProjectWizardContent implementation
	void load() override;
	void save() override;

private:
	std::shared_ptr<SourceGroupSettings> getSourceGroupSettings() override;

	std::shared_ptr<SourceGroupSettingsJavaMaven> m_settings;
};

#endif // QT_PROJECT_WIZARD_CONTENT_PATH_DEPENDENCIES_MAVEN_H
