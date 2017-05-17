#ifndef QT_PROJECT_WIZZARD_H
#define QT_PROJECT_WIZZARD_H

#include <QWidget>

#include "qt/window/QtWindowStack.h"

#include "settings/ApplicationSettings.h"
#include "settings/ProjectSettings.h"

class QtProjectWizzardContent;
class QtProjectWizzardContentSummary;
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

	void newProjectFromSolution(const std::string& ideId, const FilePath& solutionPath);
	void newProjectFromCDB(const FilePath& filePath, const std::vector<FilePath>& headerPaths);
	void refreshProjectFromSolution(const std::string& ideId, const std::string& solutionPath);

	void editProject(const FilePath& settingsPath);
	void editProject(std::shared_ptr<ProjectSettings> settings);
	void showPreferences();

private:
	static bool applicationSettingsContainVisualStudioHeaderSearchPaths();

	QtProjectWizzardWindow* createWindowWithContent(
		std::function<QtProjectWizzardContent*(QtProjectWizzardWindow*)> func);

	QtProjectWizzardWindow* createWindowWithSummary(
		std::function<void(QtProjectWizzardWindow*, QtProjectWizzardContentSummary*)> func);

	QtWindowStack m_windowStack;

	std::shared_ptr<ProjectSettings> m_projectSettings;
	std::shared_ptr<SourceGroupSettings> m_sourceGroupSettings;
	ApplicationSettings m_appSettings;

	bool m_editing;

private slots:
	void cancelWizzard();
	void finishWizzard();

	void windowStackChanged();

	void selectedProjectType(SourceGroupType sourceGroupType);

	void emptyProject();
	void emptyProjectCDBVS();
	void emptyProjectCDB();
	void emptyProjectJavaMaven();

	void sourcePaths();
	void headerSearchPaths();

	void headerSearchPathsDone();
	void frameworkSearchPaths();

	void headerPathsCDB();

	void sourcePathsJava();
	void sourcePathsJavaMaven();

	void advancedSettingsCxx();
	void advancedSettingsJava();

	void showSummary();
	void showSummaryJava();

	void createProject();
	void savePreferences();
};

#endif // QT_PROJECT_WIZZARD_H
