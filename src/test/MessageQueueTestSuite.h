#include <cxxtest/TestSuite.h>

#include <chrono>

#include "utility/messaging/Message.h"
#include "utility/messaging/MessageListener.h"
#include "utility/messaging/MessageQueue.h"

class MessageQueueTestSuite: public CxxTest::TestSuite
{
public:
	void test_message_loop_starts_and_stops(void)
	{
		TS_ASSERT(!MessageQueue::getInstance()->loopIsRunning());

		MessageQueue::getInstance()->startMessageLoopThreaded();

		waitForThread();

		TS_ASSERT(MessageQueue::getInstance()->loopIsRunning());

		MessageQueue::getInstance()->stopMessageLoop();

		waitForThread();

		TS_ASSERT(!MessageQueue::getInstance()->loopIsRunning());
	}

	void test_registered_listener_receives_messages(void)
	{
		MessageQueue::getInstance()->startMessageLoopThreaded();

		TestMessageListener listener;
		Test2MessageListener listener2;

		TestMessage().dispatch();
		TestMessage().dispatch();
		TestMessage().dispatch();

		waitForThread();

		MessageQueue::getInstance()->stopMessageLoop();

		TS_ASSERT_EQUALS(3, listener.m_messageCount);
		TS_ASSERT_EQUALS(0, listener2.m_messageCount);
	}

	void test_message_dispatching_within_message_handling(void)
	{
		MessageQueue::getInstance()->startMessageLoopThreaded();

		TestMessageListener listener;
		Test2MessageListener listener2;

		Test2Message().dispatch();

		waitForThread();

		MessageQueue::getInstance()->stopMessageLoop();

		TS_ASSERT_EQUALS(1, listener.m_messageCount);
		TS_ASSERT_EQUALS(1, listener2.m_messageCount);
	}

	void test_listener_registration_within_message_handling(void)
	{
		MessageQueue::getInstance()->startMessageLoopThreaded();

		Test3MessageListener listener;

		Test2Message().dispatch();
		TestMessage().dispatch();

		waitForThread();

		MessageQueue::getInstance()->stopMessageLoop();

		TS_ASSERT(listener.m_listener);
		if (listener.m_listener)
		{
			TS_ASSERT_EQUALS(1, listener.m_listener->m_messageCount);
		}
	}

	void test_listener_unregistration_within_message_handling(void)
	{
		MessageQueue::getInstance()->startMessageLoopThreaded();

		Test4MessageListener listener;

		TestMessage().dispatch();

		Test2Message().dispatch();

		TestMessage().dispatch();
		TestMessage().dispatch();
		TestMessage().dispatch();

		waitForThread();

		MessageQueue::getInstance()->stopMessageLoop();

		TS_ASSERT(listener.m_listener);
		if (listener.m_listener)
		{
			TS_ASSERT_EQUALS(2, listener.m_listener->m_messageCount);
		}
	}

	void test_listener_registration_to_front_and_back_within_message_handling(void)
	{
		MessageQueue::getInstance()->startMessageLoopThreaded();

		Test5MessageListener listener;

		TestMessage().dispatch();
		TestMessage().dispatch();
		TestMessage().dispatch();

		waitForThread();

		MessageQueue::getInstance()->stopMessageLoop();

		TS_ASSERT_EQUALS(5, listener.m_listeners.size());
		TS_ASSERT_EQUALS(2, listener.m_listeners[0]->m_messageCount);
		TS_ASSERT_EQUALS(2, listener.m_listeners[1]->m_messageCount);
		TS_ASSERT_EQUALS(2, listener.m_listeners[2]->m_messageCount);
		TS_ASSERT_EQUALS(2, listener.m_listeners[3]->m_messageCount);
		TS_ASSERT_EQUALS(2, listener.m_listeners[4]->m_messageCount);
	}

private:
	class TestMessage: public Message<TestMessage>
	{
	public:
		static const std::string getStaticType()
		{
			return "TestMessage";
		}
	};

	class Test2Message: public Message<Test2Message>
	{
	public:
		static const std::string getStaticType()
		{
			return "TestMessage2";
		}
	};

	class TestMessageListener: public MessageListener<TestMessage>
	{
	public:
		TestMessageListener(bool toFront = false)
			: MessageListener<TestMessage>(toFront)
			, m_messageCount(0)
		{
		}

		int m_messageCount;

	private:
		virtual void handleMessage(TestMessage* message)
		{
			m_messageCount++;
		}
	};

	class Test2MessageListener: public MessageListener<Test2Message>
	{
	public:
		Test2MessageListener()
			: m_messageCount(0)
		{
		}

		int m_messageCount;

	private:
		virtual void handleMessage(Test2Message* message)
		{
			m_messageCount++;
			TestMessage().dispatch();
		}
	};

	class Test3MessageListener: public MessageListener<Test2Message>
	{
	public:
		std::shared_ptr<TestMessageListener> m_listener;

	private:
		virtual void handleMessage(Test2Message* message)
		{
			m_listener = std::make_shared<TestMessageListener>();
		}
	};

	class Test4MessageListener:
		public MessageListener<TestMessage>,
		public MessageListener<Test2Message>
	{
	public:
		std::shared_ptr<TestMessageListener> m_listener;

	private:
		virtual void handleMessage(TestMessage* message)
		{
			if (!m_listener)
			{
				m_listener = std::make_shared<TestMessageListener>();
			}
		}

		virtual void handleMessage(Test2Message* message)
		{
			m_listener.reset();
		}
	};

	class Test5MessageListener:
		public MessageListener<TestMessage>
	{
	public:
		std::vector<std::shared_ptr<TestMessageListener>> m_listeners;

	private:
		virtual void handleMessage(TestMessage* message)
		{
			if (!m_listeners.size())
			{
				for (size_t i = 0; i < 5; i++)
				{
					m_listeners.push_back(std::make_shared<TestMessageListener>(i % 2 == 1));
				}
			}
		}
	};

	void waitForThread() const
	{
		static const int THREAD_WAIT_TIME_MS = 5;
		while (MessageQueue::getInstance()->hasMessagesQueued())
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(THREAD_WAIT_TIME_MS));
		}
	}
};
