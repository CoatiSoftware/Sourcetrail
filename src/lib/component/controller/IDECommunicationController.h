#ifndef IDE_COMMUNICATION_CONTROLLER_H
#define IDE_COMMUNICATION_CONTROLLER_H

#include <string>

#include "component/controller/Controller.h"

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

	void handleIncomingMessage(const std::string& message);

private:
	virtual void handleMessage(MessageMoveIDECursor* message);
	virtual void sendMessage(const std::string& message) const = 0;

	StorageAccess* m_storageAccess;
};

#endif // IDE_COMMUNICATION_CONTROLLER_H