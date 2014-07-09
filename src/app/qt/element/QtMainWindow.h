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

public slots:
    void about();
    void openProject(const QString &path = QString());

private:
    void setupProjectMenu();
    void setupHelpMenu();

	std::vector<std::pair<View*, QDockWidget*> > m_dockWidgets;
};

#endif // QT_MAIN_WINDOW_H
