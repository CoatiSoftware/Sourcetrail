#include "IDECommunicationController.h"

#include "component/controller/helper/NetworkProtocolHelper.h"
#include "data/access/StorageAccess.h"
#include "data/location/TokenLocationFile.h"
#include "data/location/TokenLocation.h"

#include "utility/messaging/type/MessageActivateTokenLocations.h"
#include "utility/logging/logging.h"

IDECommunicationController::IDECommunicationController(StorageAccess* storageAccess)
	: m_storageAccess(storageAccess)
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
		const unsigned int cursorColumn = parsedMessage.column;

		std::shared_ptr<TokenLocationFile> tokenLocationFile = m_storageAccess->getTokenLocationsForLinesInFile(
			parsedMessage.fileLocation, parsedMessage.row, parsedMessage.row
		);

		std::vector<Id> selectedLocationIds;
		tokenLocationFile->forEachStartTokenLocation(
			[&](TokenLocation* startLocation)
			{
				if (startLocation->getColumnNumber() < cursorColumn)
				{
					TokenLocation* endLocation = startLocation->getOtherTokenLocation();
					if (endLocation && endLocation->getColumnNumber() > cursorColumn)
					{
						selectedLocationIds.push_back(startLocation->getId());
					}
				}
			}
		);

		if (selectedLocationIds.size() > 0)
		{
			MessageActivateTokenLocations(selectedLocationIds).dispatch();
		}
	}
}

void IDECommunicationController::handleMessage(MessageMoveIDECursor* message)
{
	std::string networkMessage = NetworkProtocolHelper::buildMessage(message->FilePosition, message->Row, message->Column);

	sendMessage(networkMessage);
}