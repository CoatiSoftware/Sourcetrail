#ifndef MESSAGE_QUEUE_H
#define MESSAGE_QUEUE_H

#include <memory>
#include <mutex>
#include <queue>

class MessageBase;
class MessageListenerBase;

class MessageQueue
{
public:
	static std::shared_ptr<MessageQueue> getInstance();

	void registerListener(MessageListenerBase* listener);
	void unregisterListener(MessageListenerBase* listener);

	void pushMessage(std::shared_ptr<MessageBase> message);
	void processMessage(std::shared_ptr<MessageBase> message, bool asNextTask);

	void startMessageLoopThreaded();
	void startMessageLoop();
	void stopMessageLoop();

	bool loopIsRunning() const;
	bool hasMessagesQueued() const;

	void setSendMessagesAsTasks(bool sendMessagesAsTasks);

private:
	typedef std::queue<std::shared_ptr<MessageBase>> MessageBufferType;

	static std::shared_ptr<MessageQueue> s_instance;

	MessageQueue();
	MessageQueue(const MessageQueue&);
	void operator=(const MessageQueue&);

	void processMessages();
	void sendMessage(std::shared_ptr<MessageBase> message);
	void sendMessageAsTask(std::shared_ptr<MessageBase> message, bool asNextTask) const;

	std::shared_ptr<MessageBufferType> m_frontMessageBuffer;
	std::shared_ptr<MessageBufferType> m_backMessageBuffer;
	std::vector<MessageListenerBase*> m_listeners;

	size_t m_currentListenerIndex;
	size_t m_listenersLength;

	bool m_loopIsRunning;
	bool m_threadIsRunning;

	mutable std::mutex m_frontMessageBufferMutex;
	mutable std::mutex m_backMessageBufferMutex;
	mutable std::mutex m_listenersMutex;
	mutable std::mutex m_loopMutex;
	mutable std::mutex m_threadMutex;

	bool m_sendMessagesAsTasks;
};

#endif // MESSAGE_QUEUE_H
