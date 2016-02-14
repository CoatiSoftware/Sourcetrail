#ifndef QT_APPLICATION_SETTINGS_SCREEN_H
#define QT_APPLICATION_SETTINGS_SCREEN_H

#include <QPushButton>
#include <QWidget>

#include "utility/file/FilePath.h"

#include "qt/element/QtDirectoryListBox.h"
#include "qt/window/project_wizzard/QtProjectWizzardContentPaths.h"
#include "qt/window/QtSettingsWindow.h"

class QtApplicationSettingsScreen
	: public QtSettingsWindow
{
	Q_OBJECT

public:
	QtApplicationSettingsScreen(QWidget* parent = 0);
	QSize sizeHint() const Q_DECL_OVERRIDE;

	virtual void setup() override;

	void load();

protected:
	virtual void populateWindow(QWidget* widget) override;

private slots:
	void handleCancelButtonPress();
	void handleUpdateButtonPress();

private:
	QPushButton* m_cancelButton;
	QPushButton* m_updateButton;

	QtProjectWizzardContentPathsHeaderSearchGlobal* m_headerPaths;
	QtProjectWizzardContentPathsFrameworkSearchGlobal* m_frameworkPaths;
};

#endif //QT_APPLICATION_SETTINGS_SCREEN_H
