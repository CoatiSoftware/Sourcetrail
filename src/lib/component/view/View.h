#ifndef VIEW_H
#define VIEW_H

#include <memory>

#include "utility/math/Vector2.h"

class GuiElement;
class ViewManager;

class View
{
public:
	View(std::shared_ptr<ViewManager> viewManager, std::shared_ptr<GuiElement> rootElement, const Vec2i& minSize);
	virtual ~View();

	std::shared_ptr<GuiElement> getRootElement();

	int getMinWidth() const;
	int getMinHeight() const;
	Vec2i getMinSize() const;

private:
	std::shared_ptr<ViewManager> m_viewManager;
	std::shared_ptr<GuiElement> m_rootElement;
	Vec2i m_minSize;
};

#endif // VIEW_H
