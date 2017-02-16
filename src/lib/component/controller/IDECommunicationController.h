#ifndef IDE_COMMUNICATION_CONTROLLER_H
#define IDE_COMMUNICATION_CONTROLLER_H

#include <string>

#include "component/controller/Controller.h"
#include "component/controller/helper/NetworkProtocolHelper.h"

#include "utility/messaging/MessageListener.h"
#include "utility/messaging/type/MessageActivateTokens.h"
#include "utility/messaging/type/MessageIDECreateCDB.h"
#include "utility/messaging/type/MessageMoveIDECursor.h"
#include "utility/messaging/type/MessagePluginPortChange.h"

class StorageAccess;

class IDECommunicationController
	: public Controller
	, public MessageListener<MessageActivateTokens>
	, public MessageListener<MessageIDECreateCDB>
	, public MessageListener<MessageMoveIDECursor>
	, public MessageListener<MessagePluginPortChange>
{
public:
	IDECommunicationController(StorageAccess* storageAccess);
	virtual ~IDECommunicationController();

	virtual void clear();

	virtual void startListening() = 0;
	virtual void stopListening() = 0;
	virtual bool isListening() const = 0;

	void handleIncomingMessage(const std::string& message);

	bool getEnabled() const;
	void setEnabled(const bool enabled);

protected:
	void sendInitialPing();

private:
	void handleSetActiveTokenMessage(const NetworkProtocolHelper::SetActiveTokenMessage& message);
	void handleCreateProjectMessage(const NetworkProtocolHelper::CreateProjectMessage& message);
	void handleCreateCDBProjectMessage(const NetworkProtocolHelper::CreateCDBProjectMessage& message);
	void handlePing(const NetworkProtocolHelper::PingMessage& message);

	virtual void handleMessage(MessageActivateTokens* message); // use this to send update pings
	virtual void handleMessage(MessageIDECreateCDB* message);
	virtual void handleMessage(MessageMoveIDECursor* message);
	virtual void handleMessage(MessagePluginPortChange* message);
	virtual void sendMessage(const std::string& message) const = 0;

	void sendUpdatePing();

	StorageAccess* m_storageAccess;

	bool m_enabled;
};

#endif // IDE_COMMUNICATION_CONTROLLER_H