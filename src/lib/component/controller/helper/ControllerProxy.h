#ifndef CONTROLLER_PROXY_H
#define CONTROLLER_PROXY_H

#include <functional>

#include "utility/scheduling/TaskLambda.h"

#include "component/view/View.h"

template <typename ControllerType>
class ControllerProxy
{
public:
	ControllerProxy(View* view)
		: m_view(view)
	{
	}

	void execute(std::function<void(ControllerType*)> callback)
	{
		ControllerType* controller = m_view->getController<ControllerType>();
		if (controller)
		{
			callback(controller);
		}
	}

	void executeAsTask(std::function<void(ControllerType*)> callback)
	{
		ControllerType* controller = m_view->getController<ControllerType>();
		if (controller)
		{
			Task::dispatch(std::make_shared<TaskLambda>(
				std::bind(callback, controller)
			));
		}
	}

	template<typename FuncType, typename... Args>
	void executeAsTaskWithArgs(FuncType callback, const Args... args)
	{
		ControllerType* controller = m_view->getController<ControllerType>();
		if (controller)
		{
			Task::dispatch(std::make_shared<TaskLambda>(
				[func = std::bind(callback, controller, args...)]()
				{
					func();
				}
			));
		}
	}

private:
	View* m_view;
};

#endif // CONTROLLER_PROXY_H
