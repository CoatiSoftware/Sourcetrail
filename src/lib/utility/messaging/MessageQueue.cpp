#include "MessageQueue.h"

#include <chrono>
#include <thread>

#include "MessageBase.h"
#include "MessageFilter.h"
#include "MessageListenerBase.h"
#include "../../component/TabId.h"
#include "../scheduling/TaskGroupParallel.h"
#include "../scheduling/TaskGroupSequence.h"
#include "../scheduling/TaskLambda.h"
#include "../logging/logging.h"

std::shared_ptr<MessageQueue> MessageQueue::getInstance()
{
	if (!s_instance)
	{
		s_instance = std::shared_ptr<MessageQueue>(new MessageQueue());
	}
	return s_instance;
}

MessageQueue::~MessageQueue()
{
	std::lock_guard<std::mutex> lock(m_listenersMutex);
	for (size_t i = 0; i < m_listeners.size(); i++)
	{
		m_listeners[i]->removedListener();
	}
	m_listeners.clear();
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

			// m_currentListenerIndex and m_listenersLength need to be updated in case this happens
			// while a message is handled.
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

MessageListenerBase* MessageQueue::getListenerById(Id listenerId) const
{
	std::lock_guard<std::mutex> lock(m_listenersMutex);
	for (size_t i = 0; i < m_listeners.size(); i++)
	{
		if (m_listeners[i]->getId() == listenerId)
		{
			return m_listeners[i];
		}
	}
	return nullptr;
}

void MessageQueue::addMessageFilter(std::shared_ptr<MessageFilter> filter)
{
	m_filters.push_back(filter);
}

void MessageQueue::pushMessage(std::shared_ptr<MessageBase> message)
{
	std::lock_guard<std::mutex> lock(m_messageBufferMutex);
	m_messageBuffer.push_back(message);
}

void MessageQueue::processMessage(std::shared_ptr<MessageBase> message, bool asNextTask)
{
	if (message->isLogged())
	{
		LOG_INFO_BARE(L"send " + message->str());
	}

	if (m_sendMessagesAsTasks && message->sendAsTask())
	{
		sendMessageAsTask(message, asNextTask);
	}
	else
	{
		sendMessage(message);
	}
}

void MessageQueue::startMessageLoopThreaded()
{
	std::thread(&MessageQueue::startMessageLoop, this).detach();

	std::lock_guard<std::mutex> lock(m_threadMutex);
	m_threadIsRunning = true;
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
		processMessages();

		{
			std::lock_guard<std::mutex> lock(m_loopMutex);

			if (!m_loopIsRunning)
			{
				break;
			}
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(25));
	}

	{
		std::lock_guard<std::mutex> lock(m_threadMutex);
		if (m_threadIsRunning)
		{
			m_threadIsRunning = false;
		}
	}
}

void MessageQueue::stopMessageLoop()
{
	{
		std::lock_guard<std::mutex> lock(m_loopMutex);

		if (!m_loopIsRunning)
		{
			LOG_WARNING("Loop is not running");
		}

		m_loopIsRunning = false;
	}

	while (true)
	{
		{
			std::lock_guard<std::mutex> lock(m_threadMutex);
			if (!m_threadIsRunning)
			{
				break;
			}
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(25));
	}
}

bool MessageQueue::loopIsRunning() const
{
	std::lock_guard<std::mutex> lock(m_loopMutex);
	return m_loopIsRunning;
}

bool MessageQueue::hasMessagesQueued() const
{
	std::lock_guard<std::mutex> lock(m_messageBufferMutex);
	return m_messageBuffer.size() > 0;
}

void MessageQueue::setSendMessagesAsTasks(bool sendMessagesAsTasks)
{
	m_sendMessagesAsTasks = sendMessagesAsTasks;
}

std::shared_ptr<MessageQueue> MessageQueue::s_instance;

MessageQueue::MessageQueue()
	: m_currentListenerIndex(0)
	, m_listenersLength(0)
	, m_loopIsRunning(false)
	, m_threadIsRunning(false)
	, m_sendMessagesAsTasks(false)
{
}

void MessageQueue::processMessages()
{
	while (true)
	{
		std::shared_ptr<MessageBase> message;
		{
			std::lock_guard<std::mutex> lock(m_messageBufferMutex);

			for (std::shared_ptr<MessageFilter> filter: m_filters)
			{
				if (!m_messageBuffer.size())
				{
					break;
				}

				filter->filter(&m_messageBuffer);
			}

			if (!m_messageBuffer.size())
			{
				break;
			}

			message = m_messageBuffer.front();
			m_messageBuffer.pop_front();
		}

		processMessage(message, false);
	}
}

void MessageQueue::sendMessage(std::shared_ptr<MessageBase> message)
{
	std::lock_guard<std::mutex> lock(m_listenersMutex);

	// m_listenersLength is saved, so that new listeners registered whithin message handling don't
	// get the current message and the length can be reduced when a listener gets unregistered.
	m_listenersLength = m_listeners.size();

	// The currentListenerIndex holds the index of the current listener being handled, so it can be
	// changed when a listener gets removed while message handling.
	for (m_currentListenerIndex = 0; m_currentListenerIndex < m_listenersLength;
		 m_currentListenerIndex++)
	{
		MessageListenerBase* listener = m_listeners[m_currentListenerIndex];

		if (listener->getType() == message->getType() &&
			(message->getSchedulerId() == 0 || listener->getSchedulerId() == 0 ||
			 listener->getSchedulerId() == message->getSchedulerId()))
		{
			// The listenersMutex gets unlocked so changes to listeners are possible while message handling.
			m_listenersMutex.unlock();
			listener->handleMessageBase(message.get());
			m_listenersMutex.lock();
		}
	}
}

void MessageQueue::sendMessageAsTask(std::shared_ptr<MessageBase> message, bool asNextTask) const
{
	std::shared_ptr<TaskGroup> taskGroup;
	if (message->isParallel())
	{
		taskGroup = std::make_shared<TaskGroupParallel>();
	}
	else
	{
		taskGroup = std::make_shared<TaskGroupSequence>();
	}

	{
		std::lock_guard<std::mutex> lock(m_listenersMutex);
		for (size_t i = 0; i < m_listeners.size(); i++)
		{
			MessageListenerBase* listener = m_listeners[i];

			if (listener->getType() == message->getType() &&
				(message->getSchedulerId() == 0 || listener->getSchedulerId() == 0 ||
				 listener->getSchedulerId() == message->getSchedulerId()))
			{
				Id listenerId = listener->getId();
				taskGroup->addTask(std::make_shared<TaskLambda>([listenerId, message]() {
					MessageListenerBase* listener = MessageQueue::getInstance()->getListenerById(
						listenerId);
					if (listener)
					{
						listener->handleMessageBase(message.get());
					}
				}));
			}
		}
	}

	Id schedulerId = message->getSchedulerId();
	if (!schedulerId)
	{
		schedulerId = TabId::app();
	}

	if (asNextTask)
	{
		Task::dispatchNext(schedulerId, taskGroup);
	}
	else
	{
		Task::dispatch(schedulerId, taskGroup);
	}
}
