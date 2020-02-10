#ifndef QT_PROJECT_WIZARD_H
#define QT_PROJECT_WIZARD_H

#include <functional>

#include <QWidget>

#include "ApplicationSettings.h"
#include "ProjectSettings.h"
#include "QtProjectWizardWindow.h"
#include "QtWindow.h"

class QListWidget;
class QPushButton;
class QtProjectWizardContent;
class QtProjectWizardContentGroup;
class QtProjectWizardWindow;

class QtProjectWizard: public QtProjectWizardWindow
{
	Q_OBJECT

public:
	QtProjectWizard(QWidget* parent = nullptr);
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
	QtProjectWizardWindow* createWindowWithContent(
		std::function<QtProjectWizardContent*(QtProjectWizardWindow*)> func);

	void updateSourceGroupList();
	bool canExitContent();

	QtWindowStack m_windowStack;

	std::shared_ptr<ProjectSettings> m_projectSettings;
	std::vector<std::shared_ptr<SourceGroupSettings>> m_allSourceGroupSettings;
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

	void cancelWizard();
	void finishWizard();

	void windowStackChanged();

	void newSourceGroup();
	void newSourceGroupFromVS();
	void selectedProjectType(SourceGroupType sourceGroupType);

	void createSourceGroup(std::shared_ptr<SourceGroupSettings> settings);
	void createProject();
};

#endif	  // QT_PROJECT_WIZARD_H
