#ifndef QT_PROJECT_WIZZARD_CONTENT_SOURCE_LIST_H
#define QT_PROJECT_WIZZARD_CONTENT_SOURCE_LIST_H

#include "qt/window/project_wizzard/QtProjectWizzardContent.h"

class QLabel;
class QListView;

class QtProjectWizzardContentSourceList
	: public QtProjectWizzardContent
{
public:
	QtProjectWizzardContentSourceList(ProjectSettings* settings, QtProjectWizzardWindow* window);

	// QtSettingsWindow implementation
	virtual void populateWindow(QWidget* widget) override;

	void showFilesFromSourcePaths(const std::vector<FilePath>& sourcePaths);

private:
	QLabel* m_text;
	QListView* m_list;
};

#endif // QT_PROJECT_WIZZARD_CONTENT_SOURCE_LIST_H
