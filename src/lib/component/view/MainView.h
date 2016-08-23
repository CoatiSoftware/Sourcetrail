#ifndef MAIN_VIEW_H
#define MAIN_VIEW_H

#include <string>
#include <vector>

#include "component/view/ViewLayout.h"

class MainView
	: public ViewLayout
{
public:
	MainView();
	virtual ~MainView();

	virtual void hideStartScreen() = 0;
	virtual void setTitle(const std::string& title) = 0;
	virtual void activateWindow() = 0;
	virtual void updateRecentProjectMenu() = 0;
};

#endif // MAIN_VIEW_H
