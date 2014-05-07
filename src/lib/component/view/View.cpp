#include "component/view/View.h"
#include "component/view/ViewManager.h"

View::View(std::shared_ptr<ViewManager> viewManager, std::shared_ptr<GuiElement> rootElement, const Vec2i& minSize)
	: m_viewManager(viewManager)
	, m_rootElement(rootElement)
	, m_minSize(minSize)
{
	m_viewManager->addView(this);
}

View::~View()
{
	m_viewManager->removeView(this);
}

std::shared_ptr<GuiElement> View::getRootElement()
{
	return m_rootElement;
}

int View::getMinWidth() const
{
	return m_minSize.x;
}

int View::getMinHeight() const
{
	return m_minSize.y;
}

Vec2i View::getMinSize() const
{
	return m_minSize;
}
