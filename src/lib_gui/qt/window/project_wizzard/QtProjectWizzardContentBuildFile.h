#ifndef QT_PROJECT_WIZZARD_CONTENT_BUILD_FILE_H
#define QT_PROJECT_WIZZARD_CONTENT_BUILD_FILE_H

#include "qt/window/project_wizzard/QtProjectWizzardContent.h"
#include "qt/window/project_wizzard/QtProjectWizzardContentSelect.h"

class QtLocationPicker;
class QPushButton;

class QtProjectWizzardContentBuildFile
	: public QtProjectWizzardContent
{
	Q_OBJECT

signals:
	void refreshVisualStudioSolution(const std::string&);

public:
	QtProjectWizzardContentBuildFile(ProjectSettings* settings, QtProjectWizzardWindow* window);

	// QtProjectWizzardContent implementation
	virtual void populateForm(QFormLayout* layout) override;

	virtual void load() override;

private slots:
	void refreshClicked();

private:
	QtLocationPicker* m_picker;

	QtProjectWizzardContentSelect::ProjectType m_type;
};

#endif // QT_PROJECT_WIZZARD_CONTENT_BUILD_FILE_H
