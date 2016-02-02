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
#include "qt/window/QtLicense.h"

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


class MouseReleaseFilter
	: public QObject
{
	Q_OBJECT

public:
	MouseReleaseFilter(QObject* parent);

protected:
	bool eventFilter(QObject* obj, QEvent* event);

private:
	size_t m_backButton;
	size_t m_forwardButton;
};

class MouseWheelFilter
	: public QObject
{
	Q_OBJECT

public:
	MouseWheelFilter(QObject* parent);

protected:
	bool eventFilter(QObject* obj, QEvent* event);
};


class QtMainWindow
	: public QMainWindow
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

	void forceEnterLicense();

protected:
	bool event(QEvent* event);
	void keyPressEvent(QKeyEvent* event);

public slots:
	void pushWindow(QWidget* window);
	void popWindow();
	void clearWindows();

	void activateWindow();

	void about();
	void openSettings();
	void showLicenses();
	void enterLicense();

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

	void undo();
	void redo();
	void zoomIn();
	void zoomOut();
	void resetZoom();
	void switchColorScheme();

	void toggleView(View* view, bool fromMenu);

	void handleEscapeShortcut();

	void updateRecentProjectMenu();

	static void setWindowSettingsPath(const std::string& windowSettingsPath);

private slots:
	void toggleShowDockWidgetTitleBars();

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
	void setupViewMenu();
	void setupHelpMenu();

	void setupShortcuts();

	DockWidget* getDockWidgetForView(View* view);

	void setShowDockWidgetTitleBars(bool showTitleBars);

	std::vector<DockWidget> m_dockWidgets;
	QMenu* m_viewMenu;
	QAction* m_viewSeparator;
	QAction** m_recentProjectAction;
	QAction* m_showTitleBarsAction;
	bool m_showDockWidgetTitleBars;

	std::shared_ptr<QtApplicationSettingsScreen> m_applicationSettingsScreen;
	std::shared_ptr<QtStartScreen> m_startScreen;
	std::shared_ptr<QtProjectSetupScreen> m_newProjectDialog;
	std::shared_ptr<QtAboutLicense> m_licenseWindow;
	std::shared_ptr<QtAbout> m_aboutWindow;
	std::shared_ptr<QtLicense> m_enterLicenseWindow;

	std::vector<QWidget*> m_windowStack;

	QShortcut* m_escapeShortcut;

	static std::string m_windowSettingsPath;
};

#endif // QT_MAIN_WINDOW_H
