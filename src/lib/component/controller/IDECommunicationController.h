#ifndef IDE_COMMUNICATION_CONTROLLER_H
#define IDE_COMMUNICATION_CONTROLLER_H

#include <string>

#include "component/controller/Controller.h"

#include "component/controller/helper/NetworkProtocolHelper.h"

#include "utility/messaging/MessageListener.h"
#include "utility/messaging/type/MessageMoveIDECursor.h"

class StorageAccess;

class IDECommunicationController
	: public Controller
	, public MessageListener<MessageMoveIDECursor>
{
public:
	IDECommunicationController(StorageAccess* storageAccess);
	virtual ~IDECommunicationController();

	virtual void clear();

	void handleIncomingMessage(const std::string& message);

	bool getEnabled() const;
	void setEnabled(const bool enabled);

	virtual bool isListening() const = 0;

private:
	void handleSetActiveTokenMessage(const NetworkProtocolHelper::SetActiveTokenMessage& message);
	void handleCreateProjectMessage(const NetworkProtocolHelper::CreateProjectMessage& message);

	virtual void handleMessage(MessageMoveIDECursor* message);
	virtual void sendMessage(const std::string& message) const = 0;

	StorageAccess* m_storageAccess;

	bool m_enabled;
};

#endif // IDE_COMMUNICATION_CONTROLLER_H