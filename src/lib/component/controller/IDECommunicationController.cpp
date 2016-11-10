#include "IDECommunicationController.h"

#include "data/access/StorageAccess.h"
#include "data/location/TokenLocationFile.h"
#include "data/location/TokenLocation.h"
#include "settings/ApplicationSettings.h"

#include "utility/file/FileSystem.h"
#include "utility/messaging/type/MessageActivateTokenLocations.h"
#include "utility/messaging/type/MessageActivateWindow.h"
#include "utility/messaging/type/MessageDispatchWhenLicenseValid.h"
#include "utility/messaging/type/MessageProjectNew.h"
#include "utility/messaging/type/MessageStatus.h"
#include "utility/messaging/type/MessageActivateFile.h"

IDECommunicationController::IDECommunicationController(StorageAccess* storageAccess)
	: m_storageAccess(storageAccess)
	, m_enabled(true)
{
}

IDECommunicationController::~IDECommunicationController()
{
}

void IDECommunicationController::clear()
{
}

void IDECommunicationController::handleIncomingMessage(const std::string& message)
{
	if (m_enabled == false)
	{
		return;
	}

	NetworkProtocolHelper::MESSAGE_TYPE type = NetworkProtocolHelper::getMessageType(message);

	if (type == NetworkProtocolHelper::MESSAGE_TYPE::UNKNOWN)
	{
		LOG_ERROR_STREAM(<< "Invalid message type");
	}
	else if (type == NetworkProtocolHelper::MESSAGE_TYPE::SET_ACTIVE_TOKEN)
	{
		handleSetActiveTokenMessage(NetworkProtocolHelper::parseSetActiveTokenMessage(message));
	}
	else if (type == NetworkProtocolHelper::MESSAGE_TYPE::CREATE_CDB_MESSAGE)
	{
		handleCreateCDBProjectMessage(NetworkProtocolHelper::parseCreateCDBProjectMessage(message));
	}
	else
	{
		handleCreateProjectMessage(NetworkProtocolHelper::parseCreateProjectMessage(message));
	}
}

bool IDECommunicationController::getEnabled() const
{
	return m_enabled;
}

void IDECommunicationController::setEnabled(const bool enabled)
{
	m_enabled = enabled;
}

void IDECommunicationController::handleSetActiveTokenMessage(
	const NetworkProtocolHelper::SetActiveTokenMessage& message
)
{
	if (message.valid)
	{
		const unsigned int cursorColumn = message.column;

		if (FileSystem::getFileInfoForPath(message.fileLocation).lastWriteTime
			== m_storageAccess->getFileInfoForFilePath(message.fileLocation).lastWriteTime)
		{
			// file was not modified
			std::shared_ptr<TokenLocationFile> tokenLocationFile = m_storageAccess->getTokenLocationsForLinesInFile(
				message.fileLocation, message.row, message.row
			);

			std::vector<Id> selectedLocationIds;
			tokenLocationFile->forEachStartTokenLocation(
				[&](TokenLocation* startLocation)
				{
					TokenLocation* endLocation = startLocation->getEndTokenLocation();

					if (!startLocation->isScopeTokenLocation()
						&& startLocation->getColumnNumber() <= cursorColumn
						&& endLocation->getColumnNumber() + 1 >= cursorColumn)
					{
						selectedLocationIds.push_back(startLocation->getId());
					}
				}
			);

			if (selectedLocationIds.size() > 0)
			{
				MessageStatus("Activating a source location from plug-in succeeded.").dispatch();
				MessageDispatchWhenLicenseValid(
					std::make_shared<MessageActivateTokenLocations>(selectedLocationIds)
				).dispatch();
				MessageActivateWindow().dispatch();
				return;
			}
		}

		Id fileId = m_storageAccess->getTokenIdForFileNode(message.fileLocation);
		if (fileId > 0)
		{
			MessageDispatchWhenLicenseValid(
				std::make_shared<MessageActivateFile>(message.fileLocation, message.row)
			).dispatchImmediately();
			MessageActivateWindow().dispatch();
		}
		else
		{
			MessageStatus(
				"Activating source location from plug-in failed. File " + message.fileLocation
				+ " was not found in the project.",
				true
			).dispatch();
		}
	}
}

void IDECommunicationController::handleCreateProjectMessage(const NetworkProtocolHelper::CreateProjectMessage& message)
{
	if (message.valid)
	{
		if (message.ideId == "vs")
		{
			std::shared_ptr<MessageProjectNew> msg = std::make_shared<MessageProjectNew>();
			msg->setSolutionPath(message.solutionFileLocation);
			msg->ideId = message.ideId;
			MessageDispatchWhenLicenseValid(msg).dispatch();
		}
		else
		{
			LOG_ERROR_STREAM(<< "Unable to parse provided solution, unknown format");
		}
	}
}

void IDECommunicationController::handleCreateCDBProjectMessage(const NetworkProtocolHelper::CreateCDBProjectMessage& message)
{
	if (message.valid)
	{
		std::shared_ptr<MessageProjectNew> msg = std::make_shared<MessageProjectNew>();
		msg->setSolutionPath(message.cdbFileLocation);
		msg->setHeaderPaths(message.headerPaths);
		msg->ideId = message.ideId;

		bool foo = msg->fromCDB();

		MessageDispatchWhenLicenseValid(msg).dispatch();
	}
	else
	{
		LOG_ERROR_STREAM(<< "Unable to parse provided CDB, invalid data received");
	}
}

void IDECommunicationController::handleMessage(MessageIDECreateCDB* message)
{
	std::string networkMessage = NetworkProtocolHelper::buildCreateCDBMessage();

	MessageStatus("Requesting IDE to create CDB.").dispatch();

	sendMessage(networkMessage);
}

void IDECommunicationController::handleMessage(MessageMoveIDECursor* message)
{
	std::string networkMessage = NetworkProtocolHelper::buildSetIDECursorMessage(
		message->FilePosition, message->Row, message->Column
		);

	MessageStatus(
		"Jumping the external tool to the following location: " + message->FilePosition + ", row: " +
		std::to_string(message->Row) + ", col: " + std::to_string(message->Column)
	).dispatch();

	sendMessage(networkMessage);
}

void IDECommunicationController::handleMessage(MessagePluginPortChange* message)
{
	stopListening();
	startListening();
}
