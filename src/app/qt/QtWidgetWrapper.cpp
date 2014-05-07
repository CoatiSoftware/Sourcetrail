#include "qt/QtWidgetWrapper.h"

#include "gui/GuiElement.h"
#include "utility/logging/logging.h"

std::shared_ptr<QWidget> QtWidgetWrapper::getWidgetOfElement(std::shared_ptr<GuiElement> element)
{
	return getWidgetOfElement(element.get());
}

std::shared_ptr<QWidget> QtWidgetWrapper::getWidgetOfElement(GuiElement* element)
{
	std::shared_ptr<QWidget> widget;
	std::shared_ptr<QtWidgetWrapper> qtWidgetWrapper = std::dynamic_pointer_cast<QtWidgetWrapper>(element->getWidgetWrapper());
	if (!qtWidgetWrapper || qtWidgetWrapper.use_count() == 0)
	{
		LOG_ERROR("Trying to get the qt widget of non qt gui element.");
	}
	else
	{
		widget = qtWidgetWrapper->getWidget();
	}
	return widget;
}

QtWidgetWrapper::QtWidgetWrapper(std::shared_ptr<QWidget> widget)
	: m_widget(widget)
{
}

QtWidgetWrapper::~QtWidgetWrapper()
{
}

std::shared_ptr<QWidget> QtWidgetWrapper::getWidget()
{
	return m_widget;
}
