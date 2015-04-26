#include "utility/scheduling/SimpleTask.h"

void SimpleTask::enter()
{

}

Task::TaskState SimpleTask::update()
{
	perform();

	return Task::STATE_FINISHED;
}

void SimpleTask::exit()
{

}

void SimpleTask::interrupt()
{

}

void SimpleTask::revert()
{

}
