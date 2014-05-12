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

	void startMessageLoopThreaded();
	void startMessageLoop();
	void stopMessageLoop();

	bool loopIsRunning() const;

private:
	typedef std::queue<std::shared_ptr<MessageBase> > MessageBufferType;

	static std::shared_ptr<MessageQueue> s_instance;

	MessageQueue();
	MessageQueue(const MessageQueue&);
	void operator=(const MessageQueue&);

	void processMessages();

	std::shared_ptr<MessageBufferType> m_frontMessageBuffer;
	std::shared_ptr<MessageBufferType> m_backMessageBuffer;
	std::vector<MessageListenerBase*> m_listeners;

	size_t m_currentListenerIndex;
	size_t m_listenersLength;
	bool m_loopIsRunning;

	std::mutex m_backMessageBufferMutex;
	std::mutex m_listenersMutex;
	mutable std::mutex m_loopMutex;
};

#endif // MESSAGE_QUEUE_H
