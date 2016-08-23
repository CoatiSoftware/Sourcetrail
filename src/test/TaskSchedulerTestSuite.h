#include <cxxtest/TestSuite.h>

#include <chrono>
#include <thread>

#include "utility/scheduling/Task.h"
#include "utility/scheduling/TaskGroupSequential.h"
#include "utility/scheduling/TaskScheduler.h"

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

		task.executeTask();

		TS_ASSERT_EQUALS(3, order);

		TS_ASSERT_EQUALS(1, task.enterCallOrder);
		TS_ASSERT_EQUALS(2, task.updateCallOrder);
		TS_ASSERT_EQUALS(3, task.exitCallOrder);
		TS_ASSERT_EQUALS(0, task.interruptCallOrder);
		TS_ASSERT_EQUALS(0, task.revertCallOrder);
		TS_ASSERT_EQUALS(0, task.abortCallOrder);
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
		TS_ASSERT_EQUALS(0, task->interruptCallOrder);
		TS_ASSERT_EQUALS(0, task->revertCallOrder);
		TS_ASSERT_EQUALS(0, task->abortCallOrder);
	}

	void test_scheduled_tasks_get_interrupted_with_callbacks_in_correct_order(void)
	{
		TaskScheduler::getInstance()->startSchedulerLoopThreaded();

		int order = 0;
		std::shared_ptr<TestTask> task = std::make_shared<TestTask>(&order, -1);

		Task::dispatch(task);

		std::this_thread::sleep_for(std::chrono::milliseconds(50));

		TaskScheduler::getInstance()->interruptCurrentTask();

		waitForThread();

		TaskScheduler::getInstance()->stopSchedulerLoop();

		TS_ASSERT_EQUALS(1, task->enterCallOrder);
		TS_ASSERT_EQUALS(order - 2, task->updateCallOrder);
		TS_ASSERT_EQUALS(order - 1, task->interruptCallOrder);
		TS_ASSERT_EQUALS(order, task->exitCallOrder);
		TS_ASSERT_EQUALS(0, task->revertCallOrder);
		TS_ASSERT_EQUALS(0, task->abortCallOrder);
	}

	void test_sequential_task_group_to_process_tasks_in_correct_order(void)
	{
		TaskScheduler::getInstance()->startSchedulerLoopThreaded();

		int order = 0;
		std::shared_ptr<TestTask> task1 = std::make_shared<TestTask>(&order, 1);
		std::shared_ptr<TestTask> task2 = std::make_shared<TestTask>(&order, 1);

		std::shared_ptr<TaskGroupSequential> taskGroup = std::make_shared<TaskGroupSequential>();
		taskGroup->addTask(task1);
		taskGroup->addTask(task2);

		Task::dispatch(taskGroup);

		waitForThread();

		TaskScheduler::getInstance()->stopSchedulerLoop();

		TS_ASSERT_EQUALS(6, order);

		TS_ASSERT_EQUALS(1, task1->enterCallOrder);
		TS_ASSERT_EQUALS(2, task1->updateCallOrder);
		TS_ASSERT_EQUALS(3, task1->exitCallOrder);
		TS_ASSERT_EQUALS(0, task1->interruptCallOrder);
		TS_ASSERT_EQUALS(0, task1->revertCallOrder);
		TS_ASSERT_EQUALS(0, task1->abortCallOrder);

		TS_ASSERT_EQUALS(4, task2->enterCallOrder);
		TS_ASSERT_EQUALS(5, task2->updateCallOrder);
		TS_ASSERT_EQUALS(6, task2->exitCallOrder);
		TS_ASSERT_EQUALS(0, task2->interruptCallOrder);
		TS_ASSERT_EQUALS(0, task2->revertCallOrder);
		TS_ASSERT_EQUALS(0, task2->abortCallOrder);
	}

	void test_sequential_task_group_to_interrupt_and_revert_tasks_in_correct_order(void)
	{
		TaskScheduler::getInstance()->startSchedulerLoopThreaded();

		int order = 0;
		std::shared_ptr<TestTask> task1 = std::make_shared<TestTask>(&order, 1);
		std::shared_ptr<TestTask> task2 = std::make_shared<TestTask>(&order, -1);

		std::shared_ptr<TaskGroupSequential> taskGroup = std::make_shared<TaskGroupSequential>();
		taskGroup->addTask(task1);
		taskGroup->addTask(task2);

		Task::dispatch(taskGroup);

		std::this_thread::sleep_for(std::chrono::milliseconds(50));

		TaskScheduler::getInstance()->interruptCurrentTask();

		waitForThread();

		TaskScheduler::getInstance()->stopSchedulerLoop();

		TS_ASSERT_EQUALS(1, task1->enterCallOrder);
		TS_ASSERT_EQUALS(2, task1->updateCallOrder);
		TS_ASSERT_EQUALS(3, task1->exitCallOrder);
		TS_ASSERT_EQUALS(0, task1->interruptCallOrder);
		TS_ASSERT_EQUALS(order - 2, task1->revertCallOrder);
		TS_ASSERT_EQUALS(0, task1->abortCallOrder);

		TS_ASSERT_EQUALS(4, task2->enterCallOrder);
		TS_ASSERT_EQUALS(order - 3, task2->updateCallOrder);
		TS_ASSERT_EQUALS(order - 1, task2->interruptCallOrder);
		TS_ASSERT_EQUALS(order, task2->exitCallOrder);
		TS_ASSERT_EQUALS(0, task2->revertCallOrder);
		TS_ASSERT_EQUALS(0, task2->abortCallOrder);
	}

	void test_sequential_task_group_to_interrupt_and_revert_nested_task_groups_in_correct_order(void)
	{
		TaskScheduler::getInstance()->startSchedulerLoopThreaded();

		int order = 0;
		std::shared_ptr<TestTask> task1 = std::make_shared<TestTask>(&order, 1);
		std::shared_ptr<TestTask> task2 = std::make_shared<TestTask>(&order, 1);
		std::shared_ptr<TestTask> task3 = std::make_shared<TestTask>(&order, -1);
		std::shared_ptr<TestTask> task4 = std::make_shared<TestTask>(&order, 1);

		std::shared_ptr<TaskGroupSequential> taskGroup1 = std::make_shared<TaskGroupSequential>();
		taskGroup1->addTask(task1);
		taskGroup1->addTask(task2);

		std::shared_ptr<TaskGroupSequential> taskGroup2 = std::make_shared<TaskGroupSequential>();
		taskGroup2->addTask(task3);
		taskGroup2->addTask(task4);

		std::shared_ptr<TaskGroupSequential> taskGroup3 = std::make_shared<TaskGroupSequential>();
		taskGroup3->addTask(taskGroup1);
		taskGroup3->addTask(taskGroup2);

		Task::dispatch(taskGroup3);

		std::this_thread::sleep_for(std::chrono::milliseconds(50));

		TaskScheduler::getInstance()->interruptCurrentTask();

		waitForThread();

		TaskScheduler::getInstance()->stopSchedulerLoop();

		TS_ASSERT_EQUALS(1, task1->enterCallOrder);
		TS_ASSERT_EQUALS(2, task1->updateCallOrder);
		TS_ASSERT_EQUALS(3, task1->exitCallOrder);
		TS_ASSERT_EQUALS(0, task1->interruptCallOrder);
		TS_ASSERT_EQUALS(order - 3, task1->revertCallOrder);
		TS_ASSERT_EQUALS(0, task1->abortCallOrder);

		TS_ASSERT_EQUALS(4, task2->enterCallOrder);
		TS_ASSERT_EQUALS(5, task2->updateCallOrder);
		TS_ASSERT_EQUALS(6, task2->exitCallOrder);
		TS_ASSERT_EQUALS(0, task2->interruptCallOrder);
		TS_ASSERT_EQUALS(order - 4, task2->revertCallOrder);
		TS_ASSERT_EQUALS(0, task2->abortCallOrder);

		TS_ASSERT_EQUALS(7, task3->enterCallOrder);
		TS_ASSERT_EQUALS(order - 5, task3->updateCallOrder);
		TS_ASSERT_EQUALS(order - 2, task3->interruptCallOrder);
		TS_ASSERT_EQUALS(order - 1, task3->exitCallOrder);
		TS_ASSERT_EQUALS(0, task3->revertCallOrder);
		TS_ASSERT_EQUALS(0, task3->abortCallOrder);

		TS_ASSERT_EQUALS(0, task4->enterCallOrder);
		TS_ASSERT_EQUALS(0, task4->updateCallOrder);
		TS_ASSERT_EQUALS(0, task4->exitCallOrder);
		TS_ASSERT_EQUALS(0, task4->interruptCallOrder);
		TS_ASSERT_EQUALS(0, task4->revertCallOrder);
		TS_ASSERT_EQUALS(order, task4->abortCallOrder);
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
		TS_ASSERT_EQUALS(0, task->interruptCallOrder);
		TS_ASSERT_EQUALS(0, task->revertCallOrder);

		TS_ASSERT_EQUALS(4, task->subTask->enterCallOrder);
		TS_ASSERT_EQUALS(5, task->subTask->updateCallOrder);
		TS_ASSERT_EQUALS(6, task->subTask->exitCallOrder);
		TS_ASSERT_EQUALS(0, task->subTask->interruptCallOrder);
		TS_ASSERT_EQUALS(0, task->subTask->revertCallOrder);
	}

private:

	class TestTask: public Task
	{
	public:
		TestTask(int* orderCountPtr, int updateCount)
			: orderCount(*orderCountPtr)
			, updateCount(updateCount)
			, enterCallOrder(0)
			, updateCallOrder(0)
			, exitCallOrder(0)
			, interruptCallOrder(0)
			, revertCallOrder(0)
			, abortCallOrder(0)
		{
		}

		virtual void enter()
		{
			enterCallOrder = ++orderCount;
		}

		virtual TaskState update()
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

			return Task::STATE_FINISHED;
		}

		virtual void exit()
		{
			exitCallOrder = ++orderCount;
		}

		virtual void interrupt()
		{
			interruptCallOrder = ++orderCount;
		}

		virtual void revert()
		{
			revertCallOrder = ++orderCount;
		}

		virtual void abort()
		{
			abortCallOrder = ++orderCount;
		}

		int& orderCount;
		int updateCount;

		int enterCallOrder;
		int updateCallOrder;
		int exitCallOrder;
		int interruptCallOrder;
		int revertCallOrder;
		int abortCallOrder;
	};

	class TestTaskDispatch: public TestTask
	{
	public:
		TestTaskDispatch(int* orderCountPtr, int updateCount)
			: TestTask(orderCountPtr, updateCount)
		{
		}

		virtual TaskState update()
		{
			subTask = std::make_shared<TestTask>(&orderCount, 1);
			Task::dispatch(subTask);

			return TestTask::update();
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
