#ifndef QT_PROJECT_WIZZARD_CONTENT_CDB_SOURCE_H
#define QT_PROJECT_WIZZARD_CONTENT_CDB_SOURCE_H

#include "qt/window/project_wizzard/QtProjectWizzardContent.h"

class QtProjectWizzardContentCDBSource
	: public QtProjectWizzardContent
{
	Q_OBJECT

public:
	QtProjectWizzardContentCDBSource(std::shared_ptr<ProjectSettings> settings, QtProjectWizzardWindow* window);

	// QtProjectWizzardContent implementation
	virtual void populateWindow(QGridLayout* layout, int& row) override;

	virtual void load() override;

	virtual QStringList getFileNames() const override;
	virtual QString getFileNamesTitle() const override;
	virtual QString getFileNamesDescription() const override;

private:
	QLabel* m_text;
	QStringList m_fileNames;
};

#endif // QT_PROJECT_WIZZARD_CONTENT_CDB_SOURCE_H
