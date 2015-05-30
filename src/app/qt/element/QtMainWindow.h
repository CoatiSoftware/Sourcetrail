#ifndef QT_MAIN_WINDOW_H
#define QT_MAIN_WINDOW_H

#include <memory>
#include <utility>
#include <vector>

#include <QShortcut>
#include <QtWidgets/QMainWindow>

#include "QtStartScreen.h"

class QDockWidget;
class View;

class QtMainWindow: public QMainWindow
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

protected:
	bool event(QEvent* event);

public slots:
	void about();
	void newProject();
	void openProject(const QString &path = QString());
	void find();
	void closeWindow();
	void refresh();
	void saveProject();
	void saveAsProject();
	void showLicences();
	void undo();
	void redo();

	void handleEscapeShortcut();

private:
    void setupEditMenu();
	void setupProjectMenu();
	void setupHelpMenu();

	void setupShortcuts();

	QDockWidget* getDockWidgetForView(View* view) const;

	std::shared_ptr<QtStartScreen> m_startScreen;

	std::vector<std::pair<View*, QDockWidget*>> m_dockWidgets;

	QShortcut* m_escapeShortcut;
};

#endif // QT_MAIN_WINDOW_H
