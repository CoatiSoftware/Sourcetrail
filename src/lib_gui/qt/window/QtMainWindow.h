#ifndef QT_MAIN_WINDOW_H
#define QT_MAIN_WINDOW_H

#include <memory>
#include <utility>
#include <vector>

#include <QMainWindow>
#include <QShortcut>

#include "utility/messaging/MessageListener.h"
#include "utility/messaging/type/MessageProjectNew.h"

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
	, public MessageListener<MessageProjectNew>
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

	void handleMessage(MessageProjectNew* message);

protected:
	bool event(QEvent* event);
	void keyPressEvent(QKeyEvent* event);

public slots:
	void activateWindow();

	void about();
	void openSettings();
	void showBugtracker();
	void showDocumentation();
	void showLicenses();
	void enterLicense();

	void showStartScreen();
	void hideStartScreen();

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

	void doCreateNewProject(MessageProjectNew message);

    void setupEditMenu();
	void setupProjectMenu();
	void setupViewMenu();
	void setupHelpMenu();

	void setupShortcuts();

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

	QtWindowStack m_windowStack;

	QShortcut* m_escapeShortcut;

	QtThreadedFunctor<MessageProjectNew> m_createNewProjectFunctor;
};

#endif // QT_MAIN_WINDOW_H
