#ifndef CONTROLLER_PROXY_H
#define CONTROLLER_PROXY_H

#include <functional>

#include "utility/scheduling/TaskLambda.h"

#include "component/view/View.h"

class ControllerProxy
{
public:
	ControllerProxy(View* view)
		: m_view(view)
	{
	}

	template<typename ControllerType>
	void execute(std::function<void(ControllerType*)> callback)
	{
		ControllerType* controller = m_view->getController<ControllerType>();
		if (controller)
		{
			callback(controller);
		}
	}

	template<typename ControllerType>
	void executeAsTask(std::function<void(ControllerType*)> callback)
	{
		ControllerType* controller = m_view->getController<ControllerType>();
		if (controller)
		{
			Task::dispatch(std::make_shared<TaskLambda>(
				[callback, controller]()
				{
					callback(controller);
				}
			));
		}
	}

private:
	View* m_view;
};

#endif // CONTROLLER_PROXY_H
