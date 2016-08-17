#ifndef QT_PROJECT_WIZZARD_CONTENT_SIMPLE_H
#define QT_PROJECT_WIZZARD_CONTENT_SIMPLE_H

#include "qt/window/project_wizzard/QtProjectWizzardContent.h"

class QCheckBox;
class QButtonGroup;

class QtProjectWizzardContentSimple
	: public QtProjectWizzardContent
{
	Q_OBJECT

public:
	QtProjectWizzardContentSimple(std::shared_ptr<ProjectSettings> settings, QtProjectWizzardWindow* window);

	// QtProjectWizzardContent implementation
	virtual void populateWindow(QGridLayout* layout, int& row) override;
	virtual void populateForm(QGridLayout* layout, int& row) override;

	virtual void load() override;
	virtual void save() override;

private:
	QCheckBox* m_checkBox;
};

#endif // QT_PROJECT_WIZZARD_CONTENT_SIMPLE_H
