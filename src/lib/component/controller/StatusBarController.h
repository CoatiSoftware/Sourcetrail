#ifndef STATUS_BAR_CONTROLLER_H
#define STATUS_BAR_CONTROLLER_H

#include <string>

#include "component/controller/Controller.h"

#include "utility/messaging/MessageListener.h"
#include "utility/messaging/type/error/MessageErrorCountClear.h"
#include "utility/messaging/type/error/MessageErrorCountUpdate.h"
#include "utility/messaging/type/indexing/MessageIndexingFinished.h"
#include "utility/messaging/type/indexing/MessageIndexingStatus.h"
#include "utility/messaging/type/MessagePingReceived.h"
#include "utility/messaging/type/MessageRefresh.h"
#include "utility/messaging/type/MessageStatus.h"

class StatusBarView;
class StorageAccess;

class StatusBarController
	: public Controller
	, public MessageListener<MessageErrorCountClear>
	, public MessageListener<MessageErrorCountUpdate>
	, public MessageListener<MessageIndexingFinished>
	, public MessageListener<MessageIndexingStatus>
	, public MessageListener<MessagePingReceived>
	, public MessageListener<MessageRefresh>
	, public MessageListener<MessageStatus>
{
public:
	StatusBarController(StorageAccess* storageAccess);
	virtual ~StatusBarController(void);

	StatusBarView* getView();

	virtual void clear();

private:
	virtual void handleMessage(MessageErrorCountClear* message);
	virtual void handleMessage(MessageErrorCountUpdate* message);
	virtual void handleMessage(MessageIndexingFinished* message);
	virtual void handleMessage(MessageIndexingStatus* message);
	virtual void handleMessage(MessagePingReceived* message);
	virtual void handleMessage(MessageRefresh* message);
	virtual void handleMessage(MessageStatus* message);

	void setStatus(const std::wstring& status, bool isError, bool showLoader);

	StorageAccess* m_storageAccess;
};

#endif // STATUS_BAR_CONTROLLER_H
