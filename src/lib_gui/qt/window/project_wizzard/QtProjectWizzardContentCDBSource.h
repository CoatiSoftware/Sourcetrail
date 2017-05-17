#ifndef QT_PROJECT_WIZZARD_CONTENT_CDB_SOURCE_H
#define QT_PROJECT_WIZZARD_CONTENT_CDB_SOURCE_H

#include "qt/window/project_wizzard/QtProjectWizzardContent.h"

class SourceGroupSettings;

class QtProjectWizzardContentCDBSource
	: public QtProjectWizzardContent
{
	Q_OBJECT

public:
	QtProjectWizzardContentCDBSource(std::shared_ptr<SourceGroupSettings> settings, QtProjectWizzardWindow* window);

	// QtProjectWizzardContent implementation
	virtual void populate(QGridLayout* layout, int& row) override;

	virtual void load() override;

	virtual std::vector<std::string> getFileNames() const override;
	virtual QString getFileNamesTitle() const override;
	virtual QString getFileNamesDescription() const override;

private:
	std::shared_ptr<SourceGroupSettings> m_settings;
	QLabel* m_text;
	std::vector<std::string> m_fileNames;
};

#endif // QT_PROJECT_WIZZARD_CONTENT_CDB_SOURCE_H
