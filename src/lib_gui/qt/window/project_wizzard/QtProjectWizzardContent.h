#ifndef QT_PROJECT_WIZZARD_CONTENT_H
#define QT_PROJECT_WIZZARD_CONTENT_H

#include <QWidget>

#include "qt/window/project_wizzard/QtProjectWizzardWindow.h"
#include "settings/ProjectSettings.h"

class QFormLayout;

class QtProjectWizzardContent
	: public QWidget
{
	Q_OBJECT

public:
	QtProjectWizzardContent(ProjectSettings* settings, QtProjectWizzardWindow* window);

	virtual void populateWindow(QWidget* widget);
	virtual void populateForm(QFormLayout* layout);
	virtual void windowReady();

	virtual void load();
	virtual void save();
	virtual bool check();

protected:
	ProjectSettings* m_settings;
	QtProjectWizzardWindow* m_window;
};

#endif // QT_PROJECT_WIZZARD_CONTENT_H
