#include "component/view/ViewManager.h"

#include "gui/GuiCanvas.h"
#include "View.h"

ViewManager::ViewManager(std::shared_ptr<GuiCanvas> canvas)
	: m_canvas(canvas)
{
}

ViewManager::~ViewManager()
{
}

void ViewManager::addView(View* view)
{
	m_views.push_back(view);
	m_canvas->addChild(view->getRootElement());
}

void ViewManager::removeView(View* view)
{
	std::vector<View*>::iterator it = std::find(m_views.begin(), m_views.end(), view);
	if (it != m_views.end())
	{
		m_canvas->removeChild((*it)->getRootElement());
		m_views.erase(it);
	}
}
