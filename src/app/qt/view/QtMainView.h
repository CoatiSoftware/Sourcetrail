#ifndef QT_MAIN_VIEW_H
#define QT_MAIN_VIEW_H

#include <memory>
#include <vector>

#include "component/view/MainView.h"

class QtMainWindow;
class View;

class QtMainView: public MainView
{
public:
	QtMainView();
	virtual ~QtMainView();

	// ViewLayout implementation
	void addView(View* view);
	void removeView(View* view);

private:
	std::shared_ptr<QtMainWindow> m_window;
	std::vector<View*> m_views;
};

#endif // QT_MAIN_VIEW_H
