#ifndef QT_PROJECT_WIZARD_CONTENT_PATH_SETTINGS_MAVEN_H
#define QT_PROJECT_WIZARD_CONTENT_PATH_SETTINGS_MAVEN_H

#include "QtProjectWizardContentPath.h"

class QCheckBox;
class SourceGroupSettingsJavaMaven;

class QtProjectWizardContentPathSettingsMaven: public QtProjectWizardContentPath
{
	Q_OBJECT

public:
	QtProjectWizardContentPathSettingsMaven(
		std::shared_ptr<SourceGroupSettingsJavaMaven> settings, QtProjectWizardWindow* window);

	// QtProjectWizardContent implementation
	void populate(QGridLayout* layout, int& row) override;

	void load() override;
	void save() override;

private:
	std::shared_ptr<SourceGroupSettings> getSourceGroupSettings() override;

	std::shared_ptr<SourceGroupSettingsJavaMaven> m_settings;
};

#endif	  // QT_PROJECT_WIZARD_CONTENT_PATH_SETTINGS_MAVEN_H
