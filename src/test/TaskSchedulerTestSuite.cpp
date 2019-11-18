#include "catch.hpp"

#include <chrono>
#include <thread>

#include "Blackboard.h"
#include "Task.h"
#include "TaskGroupSelector.h"
#include "TaskGroupSequence.h"
#include "TaskScheduler.h"

namespace
{
void executeTask(Task& task)
{
	std::shared_ptr<Blackboard> blakboard = std::make_shared<Blackboard>();
	while (true)
	{
		if (task.update(blakboard) != Task::STATE_RUNNING)
		{
			return;
		}
	}
}

class TestTask: public Task
{
public:
	TestTask(int* orderCountPtr, int updateCount, TaskState returnState = STATE_SUCCESS)
		: orderCount(*orderCountPtr)
		, updateCount(updateCount)
		, returnState(returnState)
		, enterCallOrder(0)
		, updateCallOrder(0)
		, exitCallOrder(0)
		, resetCallOrder(0)
	{
	}

	virtual void doEnter(std::shared_ptr<Blackboard> blakboard)
	{
		enterCallOrder = ++orderCount;
	}

	virtual TaskState doUpdate(std::shared_ptr<Blackboard> blakboard)
	{
		updateCallOrder = ++orderCount;

		if (updateCount < 0)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
			return Task::STATE_RUNNING;
		}

		updateCount--;
		if (updateCount)
		{
			return Task::STATE_RUNNING;
		}

		return returnState;
	}

	virtual void doExit(std::shared_ptr<Blackboard> blakboard)
	{
		exitCallOrder = ++orderCount;
	}

	virtual void doReset(std::shared_ptr<Blackboard> blakboard)
	{
		resetCallOrder = ++orderCount;
	}

	int& orderCount;
	int updateCount;
	TaskState returnState;

	int enterCallOrder;
	int updateCallOrder;
	int exitCallOrder;
	int resetCallOrder;
};

class TestTaskDispatch: public TestTask
{
public:
	TestTaskDispatch(int* orderCountPtr, int updateCount, TaskScheduler* scheduler)
		: TestTask(orderCountPtr, updateCount), scheduler(scheduler)
	{
	}

	virtual TaskState doUpdate(std::shared_ptr<Blackboard> blakboard)
	{
		subTask = std::make_shared<TestTask>(&orderCount, 1);
		scheduler->pushTask(subTask);

		return TestTask::doUpdate(blakboard);
	}

	TaskScheduler* scheduler;
	std::shared_ptr<TestTask> subTask;
};

void waitForThread(TaskScheduler& scheduler)
{
	static const int THREAD_WAIT_TIME_MS = 20;
	do
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(THREAD_WAIT_TIME_MS));
	} while (scheduler.hasTasksQueued());
}
}	 // namespace

TEST_CASE("scheduler loop starts and stops")
{
	TaskScheduler scheduler(0);
	REQUIRE(!scheduler.loopIsRunning());

	scheduler.startSchedulerLoopThreaded();

	waitForThread(scheduler);

	REQUIRE(scheduler.loopIsRunning());

	scheduler.stopSchedulerLoop();

	waitForThread(scheduler);

	REQUIRE(!scheduler.loopIsRunning());
}

TEST_CASE("tasks get executed without scheduling in correct order")
{
	int order = 0;
	TestTask task(&order, 1);

	executeTask(task);

	REQUIRE(3 == order);

	REQUIRE(1 == task.enterCallOrder);
	REQUIRE(2 == task.updateCallOrder);
	REQUIRE(3 == task.exitCallOrder);
}

TEST_CASE("scheduled tasks get processed with callbacks in correct order")
{
	TaskScheduler scheduler(0);
	scheduler.startSchedulerLoopThreaded();

	int order = 0;
	std::shared_ptr<TestTask> task = std::make_shared<TestTask>(&order, 1);

	scheduler.pushTask(task);

	waitForThread(scheduler);

	scheduler.stopSchedulerLoop();

	REQUIRE(3 == order);

	REQUIRE(1 == task->enterCallOrder);
	REQUIRE(2 == task->updateCallOrder);
	REQUIRE(3 == task->exitCallOrder);
}

TEST_CASE("sequential task group to process tasks in correct order")
{
	TaskScheduler scheduler(0);
	scheduler.startSchedulerLoopThreaded();

	int order = 0;
	std::shared_ptr<TestTask> task1 = std::make_shared<TestTask>(&order, 1);
	std::shared_ptr<TestTask> task2 = std::make_shared<TestTask>(&order, 1);

	std::shared_ptr<TaskGroupSequence> taskGroup = std::make_shared<TaskGroupSequence>();
	taskGroup->addTask(task1);
	taskGroup->addTask(task2);

	scheduler.pushTask(taskGroup);

	waitForThread(scheduler);

	scheduler.stopSchedulerLoop();

	REQUIRE(6 == order);

	REQUIRE(1 == task1->enterCallOrder);
	REQUIRE(2 == task1->updateCallOrder);
	REQUIRE(3 == task1->exitCallOrder);

	REQUIRE(4 == task2->enterCallOrder);
	REQUIRE(5 == task2->updateCallOrder);
	REQUIRE(6 == task2->exitCallOrder);
}

TEST_CASE("sequential task group does not evaluate tasks after failure")
{
	TaskScheduler scheduler(0);
	scheduler.startSchedulerLoopThreaded();

	int order = 0;
	std::shared_ptr<TestTask> task1 = std::make_shared<TestTask>(&order, 1, Task::STATE_FAILURE);
	std::shared_ptr<TestTask> task2 = std::make_shared<TestTask>(&order, -1);

	std::shared_ptr<TaskGroupSequence> taskGroup = std::make_shared<TaskGroupSequence>();
	taskGroup->addTask(task1);
	taskGroup->addTask(task2);

	scheduler.pushTask(taskGroup);

	waitForThread(scheduler);

	scheduler.stopSchedulerLoop();

	REQUIRE(1 == task1->enterCallOrder);
	REQUIRE(2 == task1->updateCallOrder);
	REQUIRE(3 == task1->exitCallOrder);

	REQUIRE(0 == task2->enterCallOrder);
	REQUIRE(0 == task2->updateCallOrder);
	REQUIRE(0 == task2->exitCallOrder);
}

TEST_CASE("sequential task group does not evaluate tasks after success")
{
	TaskScheduler scheduler(0);
	scheduler.startSchedulerLoopThreaded();

	int order = 0;
	std::shared_ptr<TestTask> task1 = std::make_shared<TestTask>(&order, 1, Task::STATE_FAILURE);
	std::shared_ptr<TestTask> task2 = std::make_shared<TestTask>(&order, 1, Task::STATE_SUCCESS);
	std::shared_ptr<TestTask> task3 = std::make_shared<TestTask>(&order, -1);

	std::shared_ptr<TaskGroupSelector> taskGroup = std::make_shared<TaskGroupSelector>();
	taskGroup->addTask(task1);
	taskGroup->addTask(task2);
	taskGroup->addTask(task3);

	scheduler.pushTask(taskGroup);

	waitForThread(scheduler);

	scheduler.stopSchedulerLoop();

	REQUIRE(1 == task1->enterCallOrder);
	REQUIRE(2 == task1->updateCallOrder);
	REQUIRE(3 == task1->exitCallOrder);

	REQUIRE(4 == task2->enterCallOrder);
	REQUIRE(5 == task2->updateCallOrder);
	REQUIRE(6 == task2->exitCallOrder);

	REQUIRE(0 == task3->enterCallOrder);
	REQUIRE(0 == task3->updateCallOrder);
	REQUIRE(0 == task3->exitCallOrder);
}

TEST_CASE("task scheduling within task processing")
{
	TaskScheduler scheduler(0);
	scheduler.startSchedulerLoopThreaded();

	int order = 0;
	std::shared_ptr<TestTaskDispatch> task = std::make_shared<TestTaskDispatch>(
		&order, 1, &scheduler);

	scheduler.pushTask(task);

	waitForThread(scheduler);

	scheduler.stopSchedulerLoop();

	REQUIRE(6 == order);

	REQUIRE(1 == task->enterCallOrder);
	REQUIRE(2 == task->updateCallOrder);
	REQUIRE(3 == task->exitCallOrder);

	REQUIRE(4 == task->subTask->enterCallOrder);
	REQUIRE(5 == task->subTask->updateCallOrder);
	REQUIRE(6 == task->subTask->exitCallOrder);
}
