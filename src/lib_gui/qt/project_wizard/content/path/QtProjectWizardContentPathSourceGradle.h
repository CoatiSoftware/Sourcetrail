#ifndef QT_PROJECT_WIZARD_CONTENT_PATH_SOURCE_GRADLE_H
#define QT_PROJECT_WIZARD_CONTENT_PATH_SOURCE_GRADLE_H

#include "QtProjectWizardContentPath.h"

class QCheckBox;
class SourceGroupSettingsJavaGradle;

class QtProjectWizardContentPathSourceGradle
	: public QtProjectWizardContentPath
{
	Q_OBJECT

public:
	QtProjectWizardContentPathSourceGradle(
		std::shared_ptr<SourceGroupSettingsJavaGradle> settings, QtProjectWizardWindow* window);

	// QtProjectWizardContent implementation
	void populate(QGridLayout* layout, int& row) override;

	void load() override;
	void save() override;

	std::vector<FilePath> getFilePaths() const override;

private:
	std::shared_ptr<SourceGroupSettings> getSourceGroupSettings() override;

	std::shared_ptr<SourceGroupSettingsJavaGradle> m_settings;
	QCheckBox* m_shouldIndexTests;
};

#endif // QT_PROJECT_WIZARD_CONTENT_PATH_SOURCE_GRADLE_H
