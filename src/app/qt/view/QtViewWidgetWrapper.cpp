#include "qt/view/QtViewWidgetWrapper.h"

#include "component/view/View.h"
#include "utility/logging/logging.h"

QWidget* QtViewWidgetWrapper::getWidgetOfView(const View* view)
{
	QtViewWidgetWrapper* widgetWrapper = dynamic_cast<QtViewWidgetWrapper*>(view->getWidgetWrapper());

	if (!widgetWrapper)
	{
		LOG_ERROR("Trying to get the qt widget of non qt view.");
		return nullptr;
	}

	if (!widgetWrapper->getWidget())
	{
		LOG_ERROR("The QtViewWidgetWrapper is not holdling a QWidget.");
		return nullptr;
	}

	return widgetWrapper->getWidget();
}

QtViewWidgetWrapper::QtViewWidgetWrapper(QWidget* widget)
	: m_widget(widget)
{
}

QtViewWidgetWrapper::~QtViewWidgetWrapper()
{
}

QWidget* QtViewWidgetWrapper::getWidget()
{
	return m_widget;
}
