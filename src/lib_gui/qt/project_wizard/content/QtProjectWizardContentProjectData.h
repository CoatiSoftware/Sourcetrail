#ifndef QT_PROJECT_WIZARD_CONTENT_PROJECT_DATA_H
#define QT_PROJECT_WIZARD_CONTENT_PROJECT_DATA_H

#include "QtLocationPicker.h"
#include "QtProjectWizardContent.h"

class ProjectSettings;

class QtProjectWizardContentProjectData
	: public QtProjectWizardContent
{
	Q_OBJECT

public:
	QtProjectWizardContentProjectData(
		std::shared_ptr<ProjectSettings> projectSettings,
		QtProjectWizardWindow* window,
		bool disableNameEditing = false);

	// QtProjectWizardContent implementation
	virtual void populate(QGridLayout* layout, int& row) override;

	virtual void load() override;
	virtual void save() override;
	virtual bool check() override;

public slots:
	void onProjectNameEdited(QString text);

private:
	std::shared_ptr<ProjectSettings> m_projectSettings;

	bool m_disableNameEditing;
	QLineEdit* m_projectName;
	QtLocationPicker* m_projectFileLocation;
};

#endif // QT_PROJECT_WIZARD_CONTENT_PROJECT_DATA_H
