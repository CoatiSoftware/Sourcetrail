#ifndef QT_PROJECT_WIZZARD_CONTENT_PROJECT_DATA_H
#define QT_PROJECT_WIZZARD_CONTENT_PROJECT_DATA_H

#include "qt/element/QtLocationPicker.h"
#include "qt/window/project_wizzard/QtProjectWizzardContent.h"

class ProjectSettings;

class QtProjectWizzardContentProjectData
	: public QtProjectWizzardContent
{
	Q_OBJECT

public:
	QtProjectWizzardContentProjectData(
		std::shared_ptr<ProjectSettings> projectSettings,
		QtProjectWizzardWindow* window,
		bool disableNameEditing = false);

	// QtProjectWizzardContent implementation
	virtual void populate(QGridLayout* layout, int& row) override;

	virtual void load() override;
	virtual void save() override;
	virtual bool check() override;

private:
	std::shared_ptr<ProjectSettings> m_projectSettings;

	bool m_disableNameEditing;
	QLineEdit* m_projectName;
	QtLocationPicker* m_projectFileLocation;
};

#endif // QT_PROJECT_WIZZARD_CONTENT_PROJECT_DATA_H
