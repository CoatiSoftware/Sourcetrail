#ifndef QT_PROJECT_WIZZARD_CONTENT_SELECT_H
#define QT_PROJECT_WIZZARD_CONTENT_SELECT_H

#include "qt/window/project_wizzard/QtProjectWizzardContent.h"

class QButtonGroup;

class QtProjectWizzardContentSelect
	: public QtProjectWizzardContent
{
	Q_OBJECT

public:
	enum ProjectType : int
	{
		PROJECT_EMPTY = 0,
		PROJECT_CDB = 1,
		PROJECT_VS = 2
	};

	QtProjectWizzardContentSelect(ProjectSettings* settings, QtProjectWizzardWindow* window);

signals:
	void selected(QtProjectWizzardContentSelect::ProjectType);

protected:
	// QtProjectWizzardContent implementation
	virtual void populateWindow(QWidget* widget) override;
	virtual bool check() override;

private:
	QButtonGroup* m_buttons;
};

#endif // QT_PROJECT_WIZZARD_CONTENT_SELECT_H
