#ifndef STATUS_BAR_CONTROLLER_H
#define STATUS_BAR_CONTROLLER_H

#include <string>

#include "Controller.h"

#include "MessageListener.h"
#include "MessageErrorCountClear.h"
#include "MessageErrorCountUpdate.h"
#include "MessageIndexingFinished.h"
#include "MessageIndexingStarted.h"
#include "MessageIndexingStatus.h"
#include "MessagePingReceived.h"
#include "MessageRefresh.h"
#include "MessageStatus.h"

class StatusBarView;
class StorageAccess;

class StatusBarController
	: public Controller
	, public MessageListener<MessageErrorCountClear>
	, public MessageListener<MessageErrorCountUpdate>
	, public MessageListener<MessageIndexingFinished>
	, public MessageListener<MessageIndexingStarted>
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
	virtual void handleMessage(MessageIndexingStarted* message);
	virtual void handleMessage(MessageIndexingStatus* message);
	virtual void handleMessage(MessagePingReceived* message);
	virtual void handleMessage(MessageRefresh* message);
	virtual void handleMessage(MessageStatus* message);

	void setStatus(const std::wstring& status, bool isError, bool showLoader);

	StorageAccess* m_storageAccess;
};

#endif // STATUS_BAR_CONTROLLER_H
