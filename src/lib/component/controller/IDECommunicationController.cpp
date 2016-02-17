#include "IDECommunicationController.h"

#include "data/access/StorageAccess.h"
#include "data/location/TokenLocationFile.h"
#include "data/location/TokenLocation.h"

#include "utility/messaging/type/MessageActivateTokenLocations.h"
#include "utility/messaging/type/MessageActivateWindow.h"
#include "utility/messaging/type/MessageLoadProject.h"
#include "utility/messaging/type/MessageProjectNew.h"
#include "utility/messaging/type/MessageStatus.h"
#include "utility/logging/logging.h"

#include "settings/ProjectSettings.h"

IDECommunicationController::IDECommunicationController(StorageAccess* storageAccess)
	: m_storageAccess(storageAccess)
{
}

IDECommunicationController::~IDECommunicationController()
{
}

void IDECommunicationController::handleIncomingMessage(const std::string& message)
{
	NetworkProtocolHelper::MESSAGE_TYPE type = NetworkProtocolHelper::getMessageType(message);

	if (type == NetworkProtocolHelper::MESSAGE_TYPE::UNKNOWN)
	{
		LOG_ERROR_STREAM(<< "Invalid message type");
	}
	else if (type == NetworkProtocolHelper::MESSAGE_TYPE::SET_ACTIVE_TOKEN)
	{
		handleSetActiveTokenMessage(NetworkProtocolHelper::parseSetActiveTokenMessage(message));
	}
	else
	{
		handleCreateProjectMessage(NetworkProtocolHelper::parseCreateProjectMessage(message));
	}
}

void IDECommunicationController::handleSetActiveTokenMessage(const NetworkProtocolHelper::SetActiveTokenMessage& message)
{
	if (message.valid)
	{
		const unsigned int cursorColumn = message.column;

		std::shared_ptr<TokenLocationFile> tokenLocationFile = m_storageAccess->getTokenLocationsForLinesInFile(
			message.fileLocation, message.row, message.row
			);

		std::vector<Id> selectedLocationIds;
		tokenLocationFile->forEachStartTokenLocation(
			[&](TokenLocation* startLocation)
		{
			TokenLocation* endLocation = startLocation->getEndTokenLocation();

			if (!startLocation->isScopeTokenLocation() &&
				startLocation->getColumnNumber() <= cursorColumn && endLocation->getColumnNumber() + 1 >= cursorColumn)
			{
				selectedLocationIds.push_back(startLocation->getId());
			}
		}
		);

		if (selectedLocationIds.size() > 0)
		{
			MessageStatus("Activating a source location from external succeeded.").dispatch();
			MessageActivateTokenLocations(selectedLocationIds).dispatch();
			MessageActivateWindow().dispatch();
		}
		else
		{
			MessageStatus(
				"Activating a source location from external failed. No symbol(s) have been found at the selected location."
				).dispatch();
		}
	}
}

void IDECommunicationController::handleCreateProjectMessage(const NetworkProtocolHelper::CreateProjectMessage& message)
{
	if (message.valid)
	{
		if (message.ideId == NetworkProtocolHelper::CreateProjectMessage::IDE_ID::VS)
		{
			MessageProjectNew msg;
			msg.setVisualStudioSolutionPath(message.solutionFileLocation);
			msg.dispatch();
		}
		else
		{
			LOG_ERROR_STREAM(<< "Unable to parse provided solution, unknown format");
		}
	}
}

void IDECommunicationController::handleMessage(MessageMoveIDECursor* message)
{
	std::string networkMessage = NetworkProtocolHelper::buildMessage(
		message->FilePosition, message->Row, message->Column
		);

	MessageStatus(
		"Jumping the external tool to the following location: " + message->FilePosition + ", row: " +
		std::to_string(message->Row) + ", col: " + std::to_string(message->Column)
	).dispatch();

	sendMessage(networkMessage);
}