#ifndef QT_PROJECT_WIZARD_CONTENT_PATH_SONARGRAPH_PROJECT_H
#define QT_PROJECT_WIZARD_CONTENT_PATH_SONARGRAPH_PROJECT_H

#include "QtProjectWizardContentPath.h"
#include "SingleValueCache.h"

class SourceGroupSettingsCxxSonargraph;
class SourceGroupSettingsWithSonargraphProjectPath;

class QtProjectWizardContentPathSonargraphProject
	: public QtProjectWizardContentPath
{
	Q_OBJECT

public:
	QtProjectWizardContentPathSonargraphProject(
		std::shared_ptr<SourceGroupSettings> settings,
		std::shared_ptr<SourceGroupSettingsCxxSonargraph> settingsCxxSonargraph,
		std::shared_ptr<SourceGroupSettingsWithSonargraphProjectPath> settingsWithSonargraphProjectPath,
		QtProjectWizardWindow* window);

	// QtProjectWizardContent implementation
	void populate(QGridLayout* layout, int& row) override;

	void load() override;
	void save() override;
	bool check() override;

	std::vector<FilePath> getFilePaths() const override;
	QString getFileNamesTitle() const override;
	QString getFileNamesDescription() const override;

private slots:
	void pickedPath();

private:
	std::shared_ptr<SourceGroupSettings> getSourceGroupSettings() override;

	std::shared_ptr<SourceGroupSettings> m_settings;
	std::shared_ptr<SourceGroupSettingsCxxSonargraph> m_settingsCxxSonargraph;
	std::shared_ptr<SourceGroupSettingsWithSonargraphProjectPath> m_settingsWithSonargraphProjectPath;
	QLabel* m_fileCountLabel;
	mutable SingleValueCache<std::vector<FilePath>> m_filePaths;
};

#endif // QT_PROJECT_WIZARD_CONTENT_PATH_SONARGRAPH_PROJECT_H
