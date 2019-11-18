#ifndef STATUS_BAR_CONTROLLER_H
#define STATUS_BAR_CONTROLLER_H

#include <string>

#include "Controller.h"

#include "MessageErrorCountClear.h"
#include "MessageErrorCountUpdate.h"
#include "MessageIndexingFinished.h"
#include "MessageIndexingStarted.h"
#include "MessageIndexingStatus.h"
#include "MessageListener.h"
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

	void clear() override;

private:
	void handleMessage(MessageErrorCountClear* message) override;
	void handleMessage(MessageErrorCountUpdate* message) override;
	void handleMessage(MessageIndexingFinished* message) override;
	void handleMessage(MessageIndexingStarted* message) override;
	void handleMessage(MessageIndexingStatus* message) override;
	void handleMessage(MessagePingReceived* message) override;
	void handleMessage(MessageRefresh* message) override;
	void handleMessage(MessageStatus* message) override;

	void setStatus(const std::wstring& status, bool isError, bool showLoader);

	StorageAccess* m_storageAccess;
};

#endif	  // STATUS_BAR_CONTROLLER_H
