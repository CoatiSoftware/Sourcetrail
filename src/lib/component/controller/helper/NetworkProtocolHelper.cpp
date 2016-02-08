#include "NetworkProtocolHelper.h"

#include <sstream>
#include <string>

#include "utility/logging/logging.h"

std::string NetworkProtocolHelper::m_divider = ">>";
std::string NetworkProtocolHelper::m_setActiveTokenPrefix = "setActiveToken";
std::string NetworkProtocolHelper::m_moveCursorPrefix = "moveCursor";
std::string NetworkProtocolHelper::m_endOfMessageToken = "<EOM>";
std::string NetworkProtocolHelper::m_createProjectPrefix = "createProject";

NetworkProtocolHelper::MESSAGE_TYPE NetworkProtocolHelper::getMessageType(const std::string& message)
{
	std::vector<std::string> subMessages = divideMessage(message);

	if (subMessages.size() > 0)
	{
		if (subMessages[0] == m_setActiveTokenPrefix)
		{
			return MESSAGE_TYPE::SET_ACTIVE_TOKEN;
		}
		else if (subMessages[0] == m_createProjectPrefix)
		{
			return MESSAGE_TYPE::CREATE_PROJECT;
		}
		else
		{
			return MESSAGE_TYPE::UNKNOWN;
		}
	}
	
	return MESSAGE_TYPE::UNKNOWN;
}

NetworkProtocolHelper::SetActiveTokenMessage NetworkProtocolHelper::parseSetActiveTokenMessage(const std::string& message)
{
	std::vector<std::string> subMessages = divideMessage(message);

	SetActiveTokenMessage networkMessage;

	if (subMessages.size() > 0)
	{
		if (subMessages[0] == m_setActiveTokenPrefix)
		{
			if (subMessages.size() != 5)
			{
				LOG_ERROR_STREAM(<< "Failed to parse setActiveToken message, invalid token count");
			}
			else
			{
				std::string fileLocation = subMessages[1];
				std::string row = subMessages[2];
				std::string column = subMessages[3];

				if(
					fileLocation.length() > 0
					&& row.length() > 0
					&& column.length() > 0
					&& isDigits(row)
					&& isDigits(column)
				){
					networkMessage.fileLocation = fileLocation;
					networkMessage.row = std::stoi(row);
					networkMessage.column = std::stoi(column);
					networkMessage.valid = true;
				}
			}
		}
		else
		{
			LOG_ERROR_STREAM(<< "Failed to parse message, invalid type token");
		}
	}

	return networkMessage;
}

NetworkProtocolHelper::CreateProjectMessage NetworkProtocolHelper::parseCreateProjectMessage(const std::string& message)
{
	std::vector<std::string> subMessages = divideMessage(message);

	NetworkProtocolHelper::CreateProjectMessage networkMessage;

	if (subMessages.size() > 0)
	{
		if (subMessages[0] == m_createProjectPrefix)
		{
			if (subMessages.size() != 4)
			{
				LOG_ERROR_STREAM(<< "Failed to parse createProject message, invalid token count");
			}
			else
			{
				std::string fileLocation = subMessages[1];
				std::string ideId = subMessages[2];
				if (fileLocation.length() > 0
					&& ideId.length() > 0)
				{
					networkMessage.solutionFileLocation = fileLocation;

					if (ideId == "vs")
					{
						networkMessage.ideId = CreateProjectMessage::IDE_ID::VS;
					}
					else
					{
						networkMessage.ideId = CreateProjectMessage::IDE_ID::UNKNOWN;
					}

					networkMessage.valid = true;
				}
			}
		}
		else
		{
			LOG_ERROR_STREAM(<< "Failed to parse message, invalid type token");
		}
	}

	return networkMessage;
}

std::string NetworkProtocolHelper::buildMessage(const std::string& fileLocation, const unsigned int row, const unsigned int column)
{
	std::stringstream messageStream;

	messageStream << m_moveCursorPrefix;
	messageStream << m_divider;
	messageStream << fileLocation;
	messageStream << m_divider;
	messageStream << row;
	messageStream << m_divider;
	messageStream << column;
	messageStream << m_endOfMessageToken;

	return messageStream.str();
}

std::vector<std::string> NetworkProtocolHelper::divideMessage(const std::string& message)
{
	std::vector<std::string> result;

	std::string msg = message;
	size_t pos = msg.find(m_divider);

	while (pos != std::string::npos)
	{
		std::string subMessage = msg.substr(0, pos);
		result.push_back(subMessage);
		msg = msg.substr(pos + m_divider.size());
		pos = msg.find(m_divider);
	}

	if (msg.size() > 0)
	{
		pos = msg.find(m_endOfMessageToken);
		if (pos != std::string::npos)
		{
			std::string subMessage = msg.substr(0, pos);
			result.push_back(subMessage);
			msg = msg.substr(pos);
			result.push_back(msg);
		}
	}

	return result;
}

std::string NetworkProtocolHelper::removeEndOfMessageToken(const std::string& message)
{
	size_t pos = message.find(m_endOfMessageToken);
	if (pos != std::string::npos)
	{
		return message.substr(0, pos);
	}
	else
	{
		return message;
	}
}

std::string NetworkProtocolHelper::getSubstringAfterString(const std::string& message, const std::string& searchString, std::string& subMessage)
{
	std::string result = "";

	size_t pos = message.find(searchString);

	if(pos != std::string::npos)
	{
		result = message.substr(0, pos);
		subMessage = message.substr(pos + searchString.length());
	}

	return result;
}

bool NetworkProtocolHelper::isDigits(const std::string& text)
{
	return (text.find_first_not_of("0123456789") == std::string::npos);
}
