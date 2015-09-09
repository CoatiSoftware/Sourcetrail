#ifndef QT_MAIN_WINDOW_H
#define QT_MAIN_WINDOW_H

#include <memory>
#include <utility>
#include <vector>

#include <QMainWindow>
#include <QShortcut>

#include "qt/window/QtApplicationSettingsScreen.h"
#include "qt/window/QtStartScreen.h"
#include "qt/window/QtProjectSetupScreen.h"
#include "qt/window/QtAboutLicense.h"

class QDockWidget;
class View;

class QtMainWindow: public QMainWindow
{
	Q_OBJECT

public:
	QtMainWindow();
	~QtMainWindow();

	void init();

	void addView(View* view);
	void removeView(View* view);

	void showView(View* view);
	void hideView(View* view);

	void loadLayout();
	void saveLayout();

protected:
	bool event(QEvent* event);

public slots:
	void about();
	void hideScreens();
	void closeScreens();
	void restoreScreens();

	void openSettings();
	void showStartScreen();
	void newProject();
	void openProject(const QString &path = QString());
	void editProject();

	void find();
	void closeWindow();
	void refresh();

	void saveProject();
	void saveAsProject();

	void showLicenses();

	void undo();
	void redo();
	void zoomIn();
	void zoomOut();
	void switchColorScheme();

	void handleEscapeShortcut();

private:
    void setupEditMenu();
	void setupProjectMenu();
	void setupViewMenu();
	void setupHelpMenu();

	void setupShortcuts();

	QDockWidget* getDockWidgetForView(View* view) const;

	std::shared_ptr<QtApplicationSettingsScreen> m_applicationSettingsScreen;
	std::shared_ptr<QtStartScreen> m_startScreen;
	std::shared_ptr<QtProjectSetupScreen> m_newProjectDialog;
	std::shared_ptr<QtAboutLicense> m_licenseWindow;

	std::vector<std::pair<View*, QDockWidget*>> m_dockWidgets;

	QShortcut* m_escapeShortcut;

	bool m_startScreenWasVisible;
};

#endif // QT_MAIN_WINDOW_H
