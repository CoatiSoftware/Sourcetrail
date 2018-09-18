#include <cxxtest/TestSuite.h>

#include <chrono>
#include <thread>

#include "Blackboard.h"
#include "Task.h"
#include "TaskGroupSelector.h"
#include "TaskGroupSequence.h"
#include "TaskScheduler.h"

class TaskSchedulerTestSuite: public CxxTest::TestSuite
{
public:
	void test_scheduler_loop_starts_and_stops(void)
	{
		TS_ASSERT(!TaskScheduler::getInstance()->loopIsRunning());

		TaskScheduler::getInstance()->startSchedulerLoopThreaded();

		waitForThread();

		TS_ASSERT(TaskScheduler::getInstance()->loopIsRunning());

		TaskScheduler::getInstance()->stopSchedulerLoop();

		waitForThread();

		TS_ASSERT(!TaskScheduler::getInstance()->loopIsRunning());
	}

	void test_tasks_get_executed_without_scheduling_in_correct_order(void)
	{
		int order = 0;
		TestTask task(&order, 1);

		executeTask(task);

		TS_ASSERT_EQUALS(3, order);

		TS_ASSERT_EQUALS(1, task.enterCallOrder);
		TS_ASSERT_EQUALS(2, task.updateCallOrder);
		TS_ASSERT_EQUALS(3, task.exitCallOrder);
	}

	void test_scheduled_tasks_get_processed_with_callbacks_in_correct_order(void)
	{
		TaskScheduler::getInstance()->startSchedulerLoopThreaded();

		int order = 0;
		std::shared_ptr<TestTask> task = std::make_shared<TestTask>(&order, 1);

		Task::dispatch(task);

		waitForThread();

		TaskScheduler::getInstance()->stopSchedulerLoop();

		TS_ASSERT_EQUALS(3, order);

		TS_ASSERT_EQUALS(1, task->enterCallOrder);
		TS_ASSERT_EQUALS(2, task->updateCallOrder);
		TS_ASSERT_EQUALS(3, task->exitCallOrder);
	}

	void test_sequential_task_group_to_process_tasks_in_correct_order(void)
	{
		TaskScheduler::getInstance()->startSchedulerLoopThreaded();

		int order = 0;
		std::shared_ptr<TestTask> task1 = std::make_shared<TestTask>(&order, 1);
		std::shared_ptr<TestTask> task2 = std::make_shared<TestTask>(&order, 1);

		std::shared_ptr<TaskGroupSequence> taskGroup = std::make_shared<TaskGroupSequence>();
		taskGroup->addTask(task1);
		taskGroup->addTask(task2);

		Task::dispatch(taskGroup);

		waitForThread();

		TaskScheduler::getInstance()->stopSchedulerLoop();

		TS_ASSERT_EQUALS(6, order);

		TS_ASSERT_EQUALS(1, task1->enterCallOrder);
		TS_ASSERT_EQUALS(2, task1->updateCallOrder);
		TS_ASSERT_EQUALS(3, task1->exitCallOrder);

		TS_ASSERT_EQUALS(4, task2->enterCallOrder);
		TS_ASSERT_EQUALS(5, task2->updateCallOrder);
		TS_ASSERT_EQUALS(6, task2->exitCallOrder);
	}

	void test_sequential_task_group_does_not_evaluate_tasks_after_failure(void)
	{
		TaskScheduler::getInstance()->startSchedulerLoopThreaded();

		int order = 0;
		std::shared_ptr<TestTask> task1 = std::make_shared<TestTask>(&order, 1, Task::STATE_FAILURE);
		std::shared_ptr<TestTask> task2 = std::make_shared<TestTask>(&order, -1);

		std::shared_ptr<TaskGroupSequence> taskGroup = std::make_shared<TaskGroupSequence>();
		taskGroup->addTask(task1);
		taskGroup->addTask(task2);

		Task::dispatch(taskGroup);

		waitForThread();

		TaskScheduler::getInstance()->stopSchedulerLoop();

		TS_ASSERT_EQUALS(1, task1->enterCallOrder);
		TS_ASSERT_EQUALS(2, task1->updateCallOrder);
		TS_ASSERT_EQUALS(3, task1->exitCallOrder);

		TS_ASSERT_EQUALS(0, task2->enterCallOrder);
		TS_ASSERT_EQUALS(0, task2->updateCallOrder);
		TS_ASSERT_EQUALS(0, task2->exitCallOrder);
	}

	void test_sequential_task_group_does_not_evaluate_tasks_after_success(void)
	{
		TaskScheduler::getInstance()->startSchedulerLoopThreaded();

		int order = 0;
		std::shared_ptr<TestTask> task1 = std::make_shared<TestTask>(&order, 1, Task::STATE_FAILURE);
		std::shared_ptr<TestTask> task2 = std::make_shared<TestTask>(&order, 1, Task::STATE_SUCCESS);
		std::shared_ptr<TestTask> task3 = std::make_shared<TestTask>(&order, -1);

		std::shared_ptr<TaskGroupSelector> taskGroup = std::make_shared<TaskGroupSelector>();
		taskGroup->addTask(task1);
		taskGroup->addTask(task2);
		taskGroup->addTask(task3);

		Task::dispatch(taskGroup);

		waitForThread();

		TaskScheduler::getInstance()->stopSchedulerLoop();

		TS_ASSERT_EQUALS(1, task1->enterCallOrder);
		TS_ASSERT_EQUALS(2, task1->updateCallOrder);
		TS_ASSERT_EQUALS(3, task1->exitCallOrder);

		TS_ASSERT_EQUALS(4, task2->enterCallOrder);
		TS_ASSERT_EQUALS(5, task2->updateCallOrder);
		TS_ASSERT_EQUALS(6, task2->exitCallOrder);

		TS_ASSERT_EQUALS(0, task3->enterCallOrder);
		TS_ASSERT_EQUALS(0, task3->updateCallOrder);
		TS_ASSERT_EQUALS(0, task3->exitCallOrder);
	}

	void test_task_scheduling_within_task_processing()
	{
		TaskScheduler::getInstance()->startSchedulerLoopThreaded();

		int order = 0;
		std::shared_ptr<TestTaskDispatch> task = std::make_shared<TestTaskDispatch>(&order, 1);

		Task::dispatch(task);

		waitForThread();

		TaskScheduler::getInstance()->stopSchedulerLoop();

		TS_ASSERT_EQUALS(6, order);

		TS_ASSERT_EQUALS(1, task->enterCallOrder);
		TS_ASSERT_EQUALS(2, task->updateCallOrder);
		TS_ASSERT_EQUALS(3, task->exitCallOrder);

		TS_ASSERT_EQUALS(4, task->subTask->enterCallOrder);
		TS_ASSERT_EQUALS(5, task->subTask->updateCallOrder);
		TS_ASSERT_EQUALS(6, task->subTask->exitCallOrder);
	}

private:
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
		TestTaskDispatch(int* orderCountPtr, int updateCount)
			: TestTask(orderCountPtr, updateCount)
		{
		}

		virtual TaskState doUpdate(std::shared_ptr<Blackboard> blakboard)
		{
			subTask = std::make_shared<TestTask>(&orderCount, 1);
			Task::dispatch(subTask);

			return TestTask::doUpdate(blakboard);
		}

		std::shared_ptr<TestTask> subTask;
	};

	void waitForThread() const
	{
		static const int THREAD_WAIT_TIME_MS = 20;
		do
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(THREAD_WAIT_TIME_MS));
		}
		while (TaskScheduler::getInstance()->hasTasksQueued());
	}
};
