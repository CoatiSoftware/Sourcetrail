#ifndef QT_PROJECT_WIZZARD_CONTENT_SIMPLE_H
#define QT_PROJECT_WIZZARD_CONTENT_SIMPLE_H

#include "qt/window/project_wizzard/QtProjectWizzardContent.h"

class QButtonGroup;

class QtProjectWizzardContentSimple
	: public QtProjectWizzardContent
{
	Q_OBJECT

public:
	QtProjectWizzardContentSimple(ProjectSettings* settings, QtProjectWizzardWindow* window);

protected:
	// QtProjectWizzardContent implementation
	virtual void populateWindow(QWidget* widget) override;
	virtual void load() override;
	virtual void save() override;
	virtual bool check() override;

private:
	QButtonGroup* m_buttons;
};

#endif // QT_PROJECT_WIZZARD_CONTENT_SIMPLE_H
