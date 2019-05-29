#ifndef QT_PROJECT_WIZARD_CONTENT_PATH_DEPENDENCIES_GRADLE_H
#define QT_PROJECT_WIZARD_CONTENT_PATH_DEPENDENCIES_GRADLE_H

#include "QtProjectWizardContentPath.h"

class SourceGroupSettingsJavaGradle;

class QtProjectWizardContentPathDependenciesGradle
	: public QtProjectWizardContentPath
{
	Q_OBJECT

public:
	QtProjectWizardContentPathDependenciesGradle(
		std::shared_ptr<SourceGroupSettingsJavaGradle> settings, QtProjectWizardWindow* window);

	// QtProjectWizardContent implementation
	void load() override;
	void save() override;

private:
	std::shared_ptr<SourceGroupSettings> getSourceGroupSettings() override;

	std::shared_ptr<SourceGroupSettingsJavaGradle> m_settings;
};

#endif // QT_PROJECT_WIZARD_CONTENT_PATH_DEPENDENCIES_GRADLE_H
