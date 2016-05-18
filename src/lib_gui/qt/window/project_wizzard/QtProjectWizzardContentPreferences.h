#ifndef QT_PROJECT_WIZZARD_CONTENT_PREFERENCES_H
#define QT_PROJECT_WIZZARD_CONTENT_PREFERENCES_H

#include <QComboBox>
#include <QLineEdit>

#include "qt/window/project_wizzard/QtProjectWizzardContent.h"

class QtProjectWizzardContentPreferences
	: public QtProjectWizzardContent
{
	Q_OBJECT

public:
	QtProjectWizzardContentPreferences(ProjectSettings* settings, QtProjectWizzardWindow* window);

	// QtProjectWizzardContent implementation
	virtual void populateWindow(QGridLayout* layout) override;
	virtual void populateForm(QGridLayout* layout, int& row) override;

	virtual void load() override;
	virtual void save() override;
	virtual bool check() override;

private:
	QLineEdit* m_fontFace;
	QComboBox* m_fontSize;
	QComboBox* m_tabWidth;
	QComboBox* m_threads;
};

#endif // QT_PROJECT_WIZZARD_CONTENT_PREFERENCES_H
