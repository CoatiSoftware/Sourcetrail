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
		PROJECT_VS = 1,
		PROJECT_CDB = 2
	};

	QtProjectWizzardContentSelect(ProjectSettings* settings, QtProjectWizzardWindow* window);

	// QtProjectWizzardContent implementation
	virtual void populateWindow(QGridLayout* layout) override;

	virtual void save() override;
	virtual bool check() override;

	virtual QSize preferredWindowSize() const override;

signals:
	void selected(QtProjectWizzardContentSelect::ProjectType);

private:
	QButtonGroup* m_languages;
	QButtonGroup* m_buttons;

	QLabel* m_title;
};

#endif // QT_PROJECT_WIZZARD_CONTENT_SELECT_H
