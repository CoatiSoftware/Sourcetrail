#ifndef QT_MAIN_WINDOW_H
#define QT_MAIN_WINDOW_H

#include <memory>
#include <utility>
#include <vector>

#include <QMainWindow>

#include "qt/utility/QtThreadedFunctor.h"
#include "qt/window/QtWindowStack.h"

class QDockWidget;
class QtSettingsWindow;
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

private slots:
	void stopWaiting();

private:
	bool m_isWaiting;
};


class QtMainWindow
	: public QMainWindow
{
	Q_OBJECT

public:
	QtMainWindow();
	~QtMainWindow();

	void addView(View* view);
	void removeView(View* view);

	void showView(View* view);
	void hideView(View* view);

	void loadLayout();
	void saveLayout();

	void forceEnterLicense(bool expired);

protected:
	bool event(QEvent* event);
	void keyPressEvent(QKeyEvent* event);
	void contextMenuEvent(QContextMenuEvent* event);

public slots:
	void about();
	void openSettings();
	void showBugtracker();
	void showDocumentation();
	void showLicenses();
	void enterLicense();
	void enteredLicense();

	void showDataFolder();
	void showLogFolder();

	void showStartScreen();
	void hideStartScreen();

	void newProject();
	void newProjectFromSolution(const std::string& ideId, const std::string& solutionPath);
	void openProject(const QString &path = QString());
	void editProject();

	void find();
	void findFulltext();
	void codeReferencePrevious();
	void codeReferenceNext();
	void overview();

	void closeWindow();
	void refresh();
	void forceRefresh();

	void undo();
	void redo();
	void zoomIn();
	void zoomOut();
	void resetZoom();

	void openRecentProject();
	void updateRecentProjectMenu();

	void toggleView(View* view, bool fromMenu);

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

	void setTrialActionsEnabled(bool enabled);

	DockWidget* getDockWidgetForView(View* view);

	void setShowDockWidgetTitleBars(bool showTitleBars);

	template<typename T>
		T* createWindow();

	std::vector<DockWidget> m_dockWidgets;
	QMenu* m_viewMenu;
	QAction* m_viewSeparator;
	QAction** m_recentProjectAction;
	QAction* m_showTitleBarsAction;

	bool m_showDockWidgetTitleBars;

	std::vector<QAction*> m_trialDisabledActions;

	QtWindowStack m_windowStack;
};

#endif // QT_MAIN_WINDOW_H
