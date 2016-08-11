#ifndef QT_PROJECT_WIZZARD_CONTENT_SOURCE_LIST_H
#define QT_PROJECT_WIZZARD_CONTENT_SOURCE_LIST_H

#include "qt/window/project_wizzard/QtProjectWizzardContent.h"

class QLabel;
class QListView;

class QtProjectWizzardContentSourceList
	: public QtProjectWizzardContent
{
public:
	QtProjectWizzardContentSourceList(std::shared_ptr<ProjectSettings> settings, QtProjectWizzardWindow* window);

	// QtSettingsWindow implementation
	virtual void populateWindow(QWidget* widget) override;
	virtual void windowReady() override;

	virtual QSize preferredWindowSize() const override;

	void showFilesFromContent(QtProjectWizzardContent* content);

private:
	QLabel* m_text;
	QListView* m_list;
};

#endif // QT_PROJECT_WIZZARD_CONTENT_SOURCE_LIST_H
