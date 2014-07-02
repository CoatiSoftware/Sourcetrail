#include "qt/QtWidgetWrapper.h"

#include "component/view/View.h"
#include "utility/logging/logging.h"

QWidget* QtWidgetWrapper::getWidgetOfView(View* view)
{
	QtWidgetWrapper* widgetWrapper = dynamic_cast<QtWidgetWrapper*>(view->getWidgetWrapper());

	if (!widgetWrapper)
	{
		LOG_ERROR("Trying to get the qt widget of non qt view.");
		return nullptr;
	}

	if (!widgetWrapper->getWidget())
	{
		LOG_ERROR("The QtWidgetWrapper is not holdling a QWidget.");
		return nullptr;
	}

	return widgetWrapper->getWidget();
}

QtWidgetWrapper::QtWidgetWrapper(std::shared_ptr<QWidget> widget)
	: m_widget(widget)
{
}

QtWidgetWrapper::~QtWidgetWrapper()
{
}

QWidget* QtWidgetWrapper::getWidget()
{
	return m_widget.get();
}
