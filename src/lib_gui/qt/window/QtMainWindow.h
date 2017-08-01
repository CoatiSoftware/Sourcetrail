#ifndef QT_MAIN_WINDOW_H
#define QT_MAIN_WINDOW_H

#include <memory>
#include <utility>
#include <vector>

#include <QMainWindow>

#include "data/search/SearchMatch.h"
#include "qt/window/QtWindowStack.h"

class Bookmark;
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

	View* findFloatingView(const std::string& name) const;

	void loadLayout();
	void saveLayout();

	void loadDockWidgetLayout();
	void loadWindow(bool showStartWindow);

	void forceEnterLicense(bool expired);

	void updateHistoryMenu(const std::vector<SearchMatch>& history);
	void updateBookmarksMenu(const std::vector<std::shared_ptr<Bookmark>>& bookmarks);

	void setContentEnabled(bool enabled);

protected:
	void keyPressEvent(QKeyEvent* event);
	void contextMenuEvent(QContextMenuEvent* event);
	void closeEvent(QCloseEvent* event);
    void resizeEvent(QResizeEvent* event);

public slots:
	void about();
	void openSettings();
	void showBugtracker();
	void showDocumentation();
	void showKeyboardShortcuts();
	void showEula(bool forceAccept = false);
	void acceptedEula();
	void showLicenses();
	void enterLicense();
	void enteredLicense();

	void showDataFolder();
	void showLogFolder();

	void showStartScreen();
	void hideStartScreen();

	void newProject();
	void newProjectFromCDB(const std::string& filePath, const std::vector<std::string>& headerPaths);
	void openProject();
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

	void resetWindowLayout();

	void openRecentProject();
	void updateRecentProjectMenu();

	void toggleView(View* view, bool fromMenu);

private slots:
	void toggleShowDockWidgetTitleBars();

	void showBookmarkCreator();
	void showBookmarkBrowser();

	void openHistoryAction();
	void activateBookmarkAction();

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
	void setupHistoryMenu();
	void setupBookmarksMenu();
	void setupHelpMenu();

	void setTrialActionsEnabled(bool enabled);

	DockWidget* getDockWidgetForView(View* view);

	void setShowDockWidgetTitleBars(bool showTitleBars);

	template<typename T>
		T* createWindow();

	std::vector<DockWidget> m_dockWidgets;

	bool m_loaded;

	QMenu* m_viewMenu;
	QAction* m_viewSeparator;

	QMenu* m_historyMenu;
	std::vector<SearchMatch> m_history;

	QMenu* m_bookmarksMenu;
	std::vector<std::shared_ptr<Bookmark>> m_bookmarks;

	QAction** m_recentProjectAction;
	QAction* m_showTitleBarsAction;

	bool m_showDockWidgetTitleBars;

	std::vector<QAction*> m_trialDisabledActions;

	QtWindowStack m_windowStack;
};

#endif // QT_MAIN_WINDOW_H
