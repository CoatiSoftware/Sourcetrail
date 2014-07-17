#ifndef QT_MAIN_WINDOW_H
#define QT_MAIN_WINDOW_H

#include <utility>
#include <vector>

#include <QtWidgets/QMainWindow>

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

public slots:
	void about();
	void newProject();
	void openProject(const QString &path = QString());
	void find();

private:
	void setupProjectMenu();
	void setupFindMenu();
	void setupHelpMenu();

	QDockWidget* getDockWidgetForView(View* view) const;

	std::vector<std::pair<View*, QDockWidget*> > m_dockWidgets;
};

#endif // QT_MAIN_WINDOW_H
