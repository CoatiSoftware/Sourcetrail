#ifndef GUI_LAYOUT_H
#define GUI_LAYOUT_H

#include "gui/GuiContainer.h"

class GuiLayout: public GuiContainer
{
public:
	enum LayoutDirectionType
	{
		LAYOUT_DIRECTION_HORIZONTAL,
		LAYOUT_DIRECTION_VERTICAL
	};

	GuiLayout(std::shared_ptr<WidgetWrapper> widgetWrapper);
	virtual ~GuiLayout();

	virtual void setDirection(LayoutDirectionType direction) = 0;

	virtual void addChild(std::shared_ptr<GuiElement> element) = 0;
	virtual void removeChild(std::shared_ptr<GuiElement> element) = 0;
};

#endif // GUI_LAYOUT_H
