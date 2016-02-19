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
	QtProjectWizzardContentSimple(ProjectSettings* settings, QtProjectWizzardWindow* window);

	// QtProjectWizzardContent implementation
	virtual void populateWindow(QGridLayout* layout) override;
	virtual void populateForm(QGridLayout* layout, int& row) override;
	virtual void windowReady() override;

	virtual void load() override;
	virtual void save() override;
	virtual bool check() override;

	virtual QSize preferredWindowSize() const override;

private:
	QButtonGroup* m_buttons;
	QCheckBox* m_checkBox;

	bool m_isForm;
};

#endif // QT_PROJECT_WIZZARD_CONTENT_SIMPLE_H
