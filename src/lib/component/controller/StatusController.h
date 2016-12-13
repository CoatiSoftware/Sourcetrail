#ifndef STATUS_CONTROLLER_H
#define STATUS_CONTROLLER_H

#include "component/controller/Controller.h"

#include "utility/messaging/MessageListener.h"
#include "utility/messaging/type/MessageClearStatusView.h"
#include "utility/messaging/type/MessageShowStatus.h"
#include "utility/messaging/type/MessageStatus.h"
#include "utility/messaging/type/MessageStatusFilterChanged.h"
#include "utility/Status.h"

class StatusView;
class StorageAccess;

class StatusController
	: public Controller
	, public MessageListener<MessageClearStatusView>
	, public MessageListener<MessageShowStatus>
	, public MessageListener<MessageStatus>
	, public MessageListener<MessageStatusFilterChanged>
{
public:
	StatusController();
	~StatusController();

private:
	StatusView* getView() const;

	virtual void clear();

	virtual void handleMessage(MessageClearStatusView* message);
	virtual void handleMessage(MessageShowStatus* message);
	virtual void handleMessage(MessageStatus* message);
	virtual void handleMessage(MessageStatusFilterChanged* message);

	void addStatus(const std::vector<Status> status);

	std::vector<Status> m_status;
	StatusFilter m_statusFilter;
};

#endif // STATUS_CONTROLLER_H
