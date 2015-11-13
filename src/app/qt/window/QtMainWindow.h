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
#include "qt/window/QtAbout.h"

class QDockWidget;
class View;

class QtViewToggle
	: public QWidget
{
	Q_OBJECT

public:
	QtViewToggle(View* view, QWidget *parent = nullptr);

public slots:
	void toggledByAction();
	void toggledByUI();

private:
	View* m_view;
};

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
	void openRecentProject();

	void find();
	void closeWindow();
	void refresh();
	void forceRefresh();

	void saveProject();
	void saveAsProject();

	void showLicenses();

	void undo();
	void redo();
	void zoomIn();
	void zoomOut();
	void switchColorScheme();

	void toggleView(View* view, bool fromMenu);

	void handleEscapeShortcut();

private:
	struct DockWidget
	{
		QDockWidget* widget;
		View* view;
		QAction* action;
		QtViewToggle* toggle;
	};


    void setupEditMenu();
	void setupProjectMenu();
	void updateRecentProjectMenu();
	void setupViewMenu();
	void setupHelpMenu();

	void setupShortcuts();

	DockWidget* getDockWidgetForView(View* view);

	std::vector<DockWidget> m_dockWidgets;
	QMenu* m_viewMenu;
	QAction* m_viewSeparator;
	QAction** m_recentProjectAction;

	std::shared_ptr<QtApplicationSettingsScreen> m_applicationSettingsScreen;
	std::shared_ptr<QtStartScreen> m_startScreen;
	std::shared_ptr<QtProjectSetupScreen> m_newProjectDialog;
	std::shared_ptr<QtAboutLicense> m_licenseWindow;
	std::shared_ptr<QtAbout> m_aboutWindow;

	QShortcut* m_escapeShortcut;

	bool m_startScreenWasVisible;
};

#endif // QT_MAIN_WINDOW_H
