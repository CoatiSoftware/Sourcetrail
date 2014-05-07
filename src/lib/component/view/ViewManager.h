#ifndef VIEW_MANAGER_H
#define VIEW_MANAGER_H

#include <memory>
#include <vector>

class GuiCanvas;
class View;

class ViewManager
{
public:
	ViewManager(std::shared_ptr<GuiCanvas> canvas);
	~ViewManager();

	void addView(View* view);
	void removeView(View* view);

private:
	std::shared_ptr<GuiCanvas> m_canvas;
	std::vector<View*> m_views;
};


#endif // VIEW_MANAGER_H
