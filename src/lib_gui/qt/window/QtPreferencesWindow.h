#ifndef QT_PREFERENCES_WINDOW_H
#define QT_PREFERENCES_WINDOW_H

#include "qt/window/project_wizzard/QtProjectWizzardWindow.h"

#include "settings/ApplicationSettings.h"

class QtPreferencesWindow
	: public QtProjectWizzardWindow
{
	Q_OBJECT

public:
	QtPreferencesWindow(QWidget* parent = 0);
	virtual ~QtPreferencesWindow();

protected:
	virtual void windowReady() override;
	virtual void handleNext() override;

private:
	ApplicationSettings m_appSettings;
};

#endif // QT_PREFERENCES_WINDOW_H
