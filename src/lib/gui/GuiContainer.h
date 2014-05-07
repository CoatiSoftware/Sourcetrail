#ifndef GUI_CONTAINER_H
#define GUI_CONTAINER_H

#include <vector>

#include "gui/GuiElement.h"

class GuiContainer: public GuiElement
{
public:
	GuiContainer(std::shared_ptr<WidgetWrapper> widgetWrapper);
	virtual ~GuiContainer();

	virtual void addChild(std::shared_ptr<GuiElement> element) = 0;
	virtual void removeChild(std::shared_ptr<GuiElement> element) = 0;

};

#endif // GUI_CONTAINER_H
