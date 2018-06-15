#ifndef QT_PROJECT_WIZZARD_H
#define QT_PROJECT_WIZZARD_H

#include <functional>

#include <QWidget>

#include "qt/window/project_wizzard/QtProjectWizzardWindow.h"
#include "qt/window/QtWindow.h"

#include "settings/ApplicationSettings.h"
#include "settings/ProjectSettings.h"

class QListWidget;
class QPushButton;
class QtProjectWizzardContent;
class QtProjectWizzardContentGroup;
class QtProjectWizzardWindow;

class QtProjectWizzard
	: public QtProjectWizzardWindow
{
	Q_OBJECT

public:
	QtProjectWizzard(QWidget* parent = nullptr);
	virtual QSize sizeHint() const override;

public slots:
	void newProject();

	void newProjectFromCDB(const FilePath& filePath);

	void editProject(const FilePath& settingsPath);
	void editProject(std::shared_ptr<ProjectSettings> settings);

protected:
	virtual void populateWindow(QWidget* widget) override;
	virtual void windowReady() override;

	virtual void handlePrevious() override;

private:
	QtProjectWizzardWindow* createWindowWithContent(
		std::function<QtProjectWizzardContent*(QtProjectWizzardWindow*)> func);

	QtProjectWizzardWindow* createWindowWithContentGroup(
		std::function<void(QtProjectWizzardWindow*, QtProjectWizzardContentGroup*)> func);

	void updateSourceGroupList();
	bool canExitContent();

	QtWindowStack m_windowStack;

	std::shared_ptr<ProjectSettings> m_projectSettings;
	std::vector<std::shared_ptr<SourceGroupSettings>> m_allSourceGroupSettings;
	std::shared_ptr<SourceGroupSettings> m_newSourceGroupSettings;
	ApplicationSettings m_appSettings;

	bool m_editing;
	int m_previouslySelectedIndex;

	QPushButton* m_generalButton;
	QPushButton* m_removeButton;
	QPushButton* m_duplicateButton;
	QListWidget* m_sourceGroupList;
	QWidget* m_contentWidget;

private slots:
	void generalButtonClicked();
	void selectedSourceGroupChanged(int index);
	void selectedSourceGroupNameChanged(QString name);
	void removeSelectedSourceGroup();
	void duplicateSelectedSourceGroup();

	void cancelSourceGroup();

	void cancelWizzard();
	void finishWizzard();

	void windowStackChanged();

	void newSourceGroup();
	void selectedProjectType(SourceGroupType sourceGroupType);

	void emptySourceGroup();
	void emptySourceGroupCDBVS();
	void emptySourceGroupCDB();
	void emptySourceGroupCxxCodeblocks();
	void emptySourceGroupCxxSonargraph();
	void emptySourceGroupJavaSonargraph();

	void sourcePaths();
	void headerSearchPaths();

	void headerSearchPathsDone();
	void frameworkSearchPaths();

	void sourcePathsJava();
	void dependenciesJava();
	void sourcePathsJavaMaven();
	void sourcePathsJavaGradle();

	void advancedSettingsCxx();
	void advancedSettingsJava();

	void createSourceGroup();
	void createProject();
};

#endif // QT_PROJECT_WIZZARD_H
