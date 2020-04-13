#ifndef STATUS_BAR_CONTROLLER_H
#define STATUS_BAR_CONTROLLER_H

#include <string>

#include "Controller.h"

#include "../../utility/messaging/type/error/MessageErrorCountClear.h"
#include "../../utility/messaging/type/error/MessageErrorCountUpdate.h"
#include "../../utility/messaging/type/indexing/MessageIndexingFinished.h"
#include "../../utility/messaging/type/indexing/MessageIndexingStarted.h"
#include "../../utility/messaging/type/indexing/MessageIndexingStatus.h"
#include "../../utility/messaging/MessageListener.h"
#include "../../utility/messaging/type/plugin/MessagePingReceived.h"
#include "../../utility/messaging/type/MessageRefresh.h"
#include "../../utility/messaging/type/MessageStatus.h"

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
