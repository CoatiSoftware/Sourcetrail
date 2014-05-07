#ifndef GUI_ELEMENT_H
#define GUI_ELEMENT_H

#include <memory>

#include "utility/math/Color.h"
#include "utility/math/Vector2.h"

class WidgetWrapper;

class GuiElement
{
public:
	GuiElement(std::shared_ptr<WidgetWrapper> widgetWrapper);
	virtual ~GuiElement();

	virtual void setPosition(Vec2i position) = 0;
	virtual Vec2i getPosition() const = 0;
	virtual void setSize(Vec2i size) = 0;
	virtual Vec2i getSize() const = 0;

	virtual void setBackgroundColor(Colori color) = 0;
	void setBackgroundColor(int r, int g, int b, int a);

//protected:
	std::shared_ptr<WidgetWrapper> getWidgetWrapper();

private:
	std::shared_ptr<WidgetWrapper> m_widgetWrapper;
};

#endif // GUI_ELEMENT_H
