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

private:
	std::vector<std::pair<View*, QDockWidget*> > m_dockWidgets;
};

#endif // QT_MAIN_WINDOW_H
