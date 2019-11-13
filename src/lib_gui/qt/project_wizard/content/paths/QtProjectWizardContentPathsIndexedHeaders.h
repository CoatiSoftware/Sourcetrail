#ifndef QT_PROJECT_WIZARD_CONTENT_PATHS_INDEXED_HEADER_PATHS_H
#define QT_PROJECT_WIZARD_CONTENT_PATHS_INDEXED_HEADER_PATHS_H

#include "language_packages.h"

#if BUILD_CXX_LANGUAGE_PACKAGE

#include "QtProjectWizardContentPaths.h"

class SourceGroupSettingsCxxCdb;
class SourceGroupSettingsCxxCodeblocks;

class QtProjectWizardContentPathsIndexedHeaders
	: public QtProjectWizardContentPaths
{
	Q_OBJECT

public:
	static std::vector<FilePath> getIndexedPathsDerivedFromCodeblocksProject(std::shared_ptr<const SourceGroupSettingsCxxCodeblocks> settings);
	static std::vector<FilePath> getIndexedPathsDerivedFromCDB(std::shared_ptr<const SourceGroupSettingsCxxCdb> settings);

	QtProjectWizardContentPathsIndexedHeaders(
		std::shared_ptr<SourceGroupSettings> settings, QtProjectWizardWindow* window, std::string projectKindName);

	virtual void populate(QGridLayout* layout, int& row) override;

	// QtProjectWizardContent implementation
	virtual void load() override;
	virtual void save() override;

	virtual bool check() override;

private slots:
	void buttonClicked();
	void savedFilesDialog();

private:
	const std::string m_projectKindName;
};

#endif // BUILD_CXX_LANGUAGE_PACKAGE

#endif // QT_PROJECT_WIZARD_CONTENT_PATHS_INDEXED_HEADER_PATHS_H
