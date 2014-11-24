#ifndef QT_MAIN_VIEW_H
#define QT_MAIN_VIEW_H

#include <memory>
#include <vector>

#include <QStatusBar>
#include "component/view/MainView.h"

class QtMainWindow;
class View;

class QtMainView: public MainView
{
public:
	QtMainView();
	virtual ~QtMainView();

	// ViewLayout implementation
	virtual void addView(View* view);
	virtual void removeView(View* view);

	virtual void showView(View* view);
	virtual void hideView(View* view);

	virtual void loadLayout();
	virtual void saveLayout();

	virtual QStatusBar* getStatusBar();
	virtual void setStatusBar(QStatusBar* statusBar);

private:
	std::shared_ptr<QtMainWindow> m_window;
	std::vector<View*> m_views;
};

#endif // QT_MAIN_VIEW_H
