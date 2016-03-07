#ifndef STATUS_BAR_CONTROLLER_H
#define STATUS_BAR_CONTROLLER_H

#include <string>

#include "component/controller/Controller.h"

#include "utility/messaging/MessageListener.h"
#include "utility/messaging/type/MessageClearErrorCount.h"
#include "utility/messaging/type/MessageFinishedParsing.h"
#include "utility/messaging/type/MessageShowErrors.h"
#include "utility/messaging/type/MessageStatus.h"

class StatusBarView;
class StorageAccess;

class StatusBarController
	: public Controller
	, public MessageListener<MessageClearErrorCount>
	, public MessageListener<MessageFinishedParsing>
	, public MessageListener<MessageShowErrors>
	, public MessageListener<MessageStatus>
{
public:
	StatusBarController(StorageAccess* storageAccess);
	virtual ~StatusBarController(void);

	StatusBarView* getView();

private:
	virtual void handleMessage(MessageClearErrorCount* message);
	virtual void handleMessage(MessageFinishedParsing* message);
	virtual void handleMessage(MessageShowErrors* message);
	virtual void handleMessage(MessageStatus* message);

	void setStatus(const std::string& status, bool isError, bool showLoader);

	StorageAccess* m_storageAccess;
};

#endif // STATUS_BAR_CONTROLLER_H
