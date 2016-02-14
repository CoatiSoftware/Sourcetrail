#ifndef QT_PROJECT_WIZZARD_H
#define QT_PROJECT_WIZZARD_H

#include <QWidget>

#include "qt/window/project_wizzard/QtProjectWizzardContentSelect.h"
#include "qt/window/QtWindowStack.h"

class ProjectSettings;
class QtProjectWizzardWindow;

class QtProjectWizzard
	: public QWidget
{
	Q_OBJECT

signals:
	void finished();
	void canceled();

public:
	QtProjectWizzard(QWidget* parent = nullptr);

public slots:
	void newProject();
	void editProject(const ProjectSettings& settings);

private:
	template<typename T>
		QtProjectWizzardWindow* createWindowWithContent();

	QtWindowStack m_windowStack;
	ProjectSettings m_settings;

private slots:
	void cancelWizzard();
	void windowStackChanged();

	void selectedProjectType(QtProjectWizzardContentSelect::ProjectType type);

	void emptyProject();
	void simpleSetup();
	void sourcePaths();
	void headerSearchPaths();
	void frameworkSearchPaths();

	void showSummary();

	void createProject();
};

#endif // QT_PROJECT_WIZZARD_H
