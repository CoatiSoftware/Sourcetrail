#include "IDECommunicationController.h"

#include "utility/messaging/type/MessageActivateTokens.h"
#include "utility/logging/logging.h"

#include "component/controller/helper/NetworkProtocolHelper.h"

IDECommunicationController::IDECommunicationController()
{
}

IDECommunicationController::~IDECommunicationController()
{
}

void IDECommunicationController::handleIncomingMessage(const std::string& message)
{
	LOG_WARNING_STREAM(<< message);

	NetworkProtocolHelper::NetworkMessage parsedMessage = NetworkProtocolHelper::parseMessage(message);
	
	if (parsedMessage.valid)
	{
		// TODO: set active token
	}
}

void IDECommunicationController::handleMessage(MessageMoveIDECursor* message)
{
	std::string networkMessage = NetworkProtocolHelper::buildMessage(message->FilePosition, message->Row, message->Column);

	sendMessage(networkMessage);
}