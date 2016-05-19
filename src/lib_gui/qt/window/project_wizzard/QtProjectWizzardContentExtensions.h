#ifndef QT_PROJECT_WIZZARD_CONTENT_EXTENSIONS_H
#define QT_PROJECT_WIZZARD_CONTENT_EXTENSIONS_H

#include "qt/window/project_wizzard/QtProjectWizzardContent.h"

class QtDirectoryListBox;

class QtProjectWizzardContentExtensions
	: public QtProjectWizzardContent
{
	Q_OBJECT

public:
	QtProjectWizzardContentExtensions(ProjectSettings* settings, QtProjectWizzardWindow* window);

	// QtProjectWizzardContent implementation
	virtual void populateWindow(QGridLayout* layout, int& row) override;
	virtual void populateForm(QGridLayout* layout, int& row) override;

	virtual void load() override;
	virtual void save() override;

private:
	QtDirectoryListBox* m_sourceList;
};

#endif // QT_PROJECT_WIZZARD_CONTENT_EXTENSIONS_H
