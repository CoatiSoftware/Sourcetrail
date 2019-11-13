#ifndef QT_PROJECT_WIZARD_CONTENT_PATH_SOURCE_MAVEN_H
#define QT_PROJECT_WIZARD_CONTENT_PATH_SOURCE_MAVEN_H

#include "language_packages.h"

#if BUILD_JAVA_LANGUAGE_PACKAGE

#include "QtProjectWizardContentPath.h"

class QCheckBox;
class SourceGroupSettingsJavaMaven;

class QtProjectWizardContentPathSourceMaven
	: public QtProjectWizardContentPath
{
	Q_OBJECT

public:
	QtProjectWizardContentPathSourceMaven(
		std::shared_ptr<SourceGroupSettingsJavaMaven> settings, QtProjectWizardWindow* window);

	// QtProjectWizardContent implementation
	void populate(QGridLayout* layout, int& row) override;

	void load() override;
	void save() override;

	std::vector<FilePath> getFilePaths() const override;

private:
	std::shared_ptr<SourceGroupSettings> getSourceGroupSettings() override;

	std::shared_ptr<SourceGroupSettingsJavaMaven> m_settings;
	QCheckBox* m_shouldIndexTests;
};

#endif // BUILD_JAVA_LANGUAGE_PACKAGE

#endif // QT_PROJECT_WIZARD_CONTENT_PATH_SOURCE_MAVEN_H
