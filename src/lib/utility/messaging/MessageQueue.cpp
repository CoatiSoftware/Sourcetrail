#include "utility/messaging/MessageQueue.h"

#include <thread>

#include "utility/logging/logging.h"
#include "utility/messaging/MessageBase.h"
#include "utility/messaging/MessageListenerBase.h"

std::shared_ptr<MessageQueue> MessageQueue::getInstance()
{
	if (!s_instance)
	{
		s_instance = std::shared_ptr<MessageQueue>(new MessageQueue());
	}

	return s_instance;
}

void MessageQueue::registerListener(MessageListenerBase* listener)
{
	std::lock_guard<std::mutex> lock(m_listenersMutex);
	m_listeners.push_back(listener);
}

void MessageQueue::unregisterListener(MessageListenerBase* listener)
{
	std::lock_guard<std::mutex> lock(m_listenersMutex);
	for (size_t i = 0; i < m_listeners.size(); i++)
	{
		if (m_listeners[i] == listener)
		{
			m_listeners.erase(m_listeners.begin() + i);

			// m_currentListenerIndex and m_listenersLength need to be updated in case this happens while a message is
			// handled.
			if (i <= m_currentListenerIndex)
			{
				m_currentListenerIndex--;
			}

			if (i < m_listenersLength)
			{
				m_listenersLength--;
			}

			return;
		}
	}

	LOG_ERROR("Listener was not found");
}

void MessageQueue::pushMessage(std::shared_ptr<MessageBase> message)
{
	std::lock_guard<std::mutex> lock(m_backMessageBufferMutex);
	m_backMessageBuffer->push(message);
}

void MessageQueue::startMessageLoopThreaded()
{
	std::thread(&MessageQueue::startMessageLoop, this).detach();
}

void MessageQueue::startMessageLoop()
{
	{
		std::lock_guard<std::mutex> lock(m_loopMutex);

		if (m_loopIsRunning)
		{
			LOG_ERROR("Loop is already running");
			return;
		}

		m_loopIsRunning = true;
	}

	while (true)
	{
		{
			std::lock_guard<std::mutex> lock(m_loopMutex);

			if (!m_loopIsRunning)
			{
				return;
			}
		}

		processMessages();
	}
}

void MessageQueue::stopMessageLoop()
{
	std::lock_guard<std::mutex> lock(m_loopMutex);

	if (!m_loopIsRunning)
	{
		LOG_WARNING("Loop is not running");
	}

	m_loopIsRunning = false;
}

bool MessageQueue::loopIsRunning() const
{
	std::lock_guard<std::mutex> lock(m_loopMutex);
	return m_loopIsRunning;
}

std::shared_ptr<MessageQueue> MessageQueue::s_instance;

MessageQueue::MessageQueue()
	: m_currentListenerIndex(0)
	, m_listenersLength(0)
	, m_loopIsRunning(false)
{
	m_frontMessageBuffer = std::make_shared<MessageBufferType>();
	m_backMessageBuffer = std::make_shared<MessageBufferType>();
}

void MessageQueue::processMessages()
{
	{
		std::lock_guard<std::mutex> lock(m_backMessageBufferMutex);
		m_backMessageBuffer.swap(m_frontMessageBuffer);
	}

	while (m_frontMessageBuffer->size())
	{
		std::shared_ptr<MessageBase> message = m_frontMessageBuffer->front();
		m_frontMessageBuffer->pop();

		std::lock_guard<std::mutex> lock(m_listenersMutex);

		// m_listenersLength is saved, so that new listeners registered whithin message handling don't get the
		// current message and the length can be reduced when a listener gets unregistered.
		m_listenersLength = m_listeners.size();

		// The currentListenerIndex holds the index of the current listener being handled, so it can be changed when a
		// listener gets removed while message handling.
		for (m_currentListenerIndex = 0; m_currentListenerIndex < m_listenersLength; m_currentListenerIndex++)
		{
			MessageListenerBase* listener = m_listeners[m_currentListenerIndex];

			if (listener->getType() == message->getType())
			{
				// The listenersMutex gets unlocked so changes to listeners are possible while message handling.
				m_listenersMutex.unlock();
				listener->handleMessageBase(message.get());
				m_listenersMutex.lock();
			}
		}
	}
}
