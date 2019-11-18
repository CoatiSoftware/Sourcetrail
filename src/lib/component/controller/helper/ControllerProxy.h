#ifndef CONTROLLER_PROXY_H
#define CONTROLLER_PROXY_H

#include <functional>

#include "TaskLambda.h"

#include "View.h"

template <typename ControllerType>
class ControllerProxy
{
public:
	ControllerProxy(View* view, Id schedulerId): m_view(view), m_schedulerId(schedulerId) {}

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
			Task::dispatch(
				m_schedulerId, std::make_shared<TaskLambda>(std::bind(callback, controller)));
		}
	}

	template <typename FuncType, typename... Args>
	void executeAsTaskWithArgs(FuncType callback, const Args... args)
	{
		ControllerType* controller = m_view->getController<ControllerType>();
		if (controller)
		{
			Task::dispatch(
				m_schedulerId,
				std::make_shared<TaskLambda>(
					[func = std::bind(callback, controller, args...)]() { func(); }));
		}
	}

private:
	View* m_view;
	Id m_schedulerId;
};

#endif	  // CONTROLLER_PROXY_H
