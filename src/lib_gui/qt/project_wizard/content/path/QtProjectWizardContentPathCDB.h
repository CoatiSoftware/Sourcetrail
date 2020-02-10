#ifndef QT_PROJECT_WIZARD_CONTENT_PATH_CDB_H
#define QT_PROJECT_WIZARD_CONTENT_PATH_CDB_H

#include "QtProjectWizardContentPath.h"
#include "SingleValueCache.h"

class SourceGroupSettingsCxxCdb;

class QtProjectWizardContentPathCDB: public QtProjectWizardContentPath
{
	Q_OBJECT

public:
	QtProjectWizardContentPathCDB(
		std::shared_ptr<SourceGroupSettingsCxxCdb> settings, QtProjectWizardWindow* window);

	// QtProjectWizardContent implementation
	void populate(QGridLayout* layout, int& row) override;

	void load() override;
	void save() override;
	void refresh() override;

	std::vector<FilePath> getFilePaths() const override;
	QString getFileNamesTitle() const override;
	QString getFileNamesDescription() const override;

private slots:
	void pickedPath();
	void onPickerTextChanged(const QString& text);

private:
	std::shared_ptr<SourceGroupSettings> getSourceGroupSettings() override;

	std::shared_ptr<SourceGroupSettingsCxxCdb> m_settings;
	QLabel* m_fileCountLabel;
	mutable SingleValueCache<std::vector<FilePath>> m_filePaths;
};

#endif	  // QT_PROJECT_WIZARD_CONTENT_PATH_CDB_H
