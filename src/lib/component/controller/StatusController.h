#ifndef STATUS_CONTROLLER_H
#define STATUS_CONTROLLER_H

#include <mutex>

#include "component/controller/Controller.h"

#include "utility/logging/Logger.h"
#include "utility/logging/LogMessage.h"
#include "utility/messaging/MessageListener.h"
#include "utility/messaging/type/MessageClearStatusView.h"
#include "utility/messaging/type/MessageStatusFilterChanged.h"
#include "utility/messaging/type/MessageStatus.h"
#include "utility/Status.h"

class StatusView;
class StorageAccess;

class StatusController
	: public Controller
	, public MessageListener<MessageClearStatusView>
	, public MessageListener<MessageStatusFilterChanged>
	, public MessageListener<MessageStatus>
{
public:
	StatusController();
	~StatusController();

	void setEnabled(bool enabled);
	bool getEnabled() const;

private:
	bool m_enabled;
	StatusView* getView() const;

	virtual void clear();

	virtual void handleMessage(MessageClearStatusView* message);
	virtual void handleMessage(MessageStatusFilterChanged* message);
	virtual void handleMessage(MessageStatus* message);


	void addStatus(Logger::LogLevel type, const LogMessage& message);
	void syncStatus();
	void clearTable();


	std::vector<Status> m_status;
	size_t m_lastSyncedStatus;
	StatusFilter m_statusFilter;

	std::mutex m_statusMutex;
	bool m_waiting;

};

#endif // STATUS_CONTROLLER_H
