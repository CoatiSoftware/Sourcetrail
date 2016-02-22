#ifndef QT_PROJECT_WIZZARD_H
#define QT_PROJECT_WIZZARD_H

#include <QWidget>

#include "qt/window/project_wizzard/QtProjectWizzardContentSelect.h"
#include "qt/window/QtWindowStack.h"

class ProjectSettings;
class QtProjectWizzardWindow;

class QtProjectWizzard
	: public QtWindowStackElement
{
	Q_OBJECT

signals:
	void finished();
	void canceled();

public:
	QtProjectWizzard(QWidget* parent = nullptr);

	// QtWindowStackElement implementation
	virtual void showWindow() override;
	virtual void hideWindow() override;

public slots:
	void newProject();
	void newProjectFromVisualStudioSolution(const std::string& visualStudioSolutionPath);
	void refreshProjectFromVisualStudioSolution(const std::string& visualStudioSolutionPath);
	void editProject(const ProjectSettings& settings);
	void showPreferences();

private:
	template<typename T>
		QtProjectWizzardWindow* createWindowWithContent();
	template<typename T>
		QtProjectWizzardWindow* createPopupWithContent();

	ProjectSettings getSettingsForVisualStudioSolution(const std::string& visualStudioSolutionPath) const;

	QtWindowStack m_windowStack;
	std::shared_ptr<QtProjectWizzardWindow> m_popup;
	ProjectSettings m_settings;

private slots:
	void cancelWizzard();
	void windowStackChanged();
	void popupClosed();

	void selectedProjectType(QtProjectWizzardContentSelect::ProjectType type);

	void emptyProject();
	void simpleSetup();
	void simpleSetupDone();

	void sourcePaths();
	void headerSearchPaths();

	void simpleSourcePaths();
	void simpleHeaderSearchPaths();

	void headerSearchPathsDone();
	void frameworkSearchPaths();

	void showSourceFiles();
	void showSummary();

	void createProject();
};

#endif // QT_PROJECT_WIZZARD_H
