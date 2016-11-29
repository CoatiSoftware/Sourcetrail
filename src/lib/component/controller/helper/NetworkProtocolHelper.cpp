#include "NetworkProtocolHelper.h"

#include <sstream>
#include <string>

#include <boost/algorithm/string.hpp>

#include "utility/logging/logging.h"

std::string NetworkProtocolHelper::m_divider = ">>";
std::string NetworkProtocolHelper::m_setActiveTokenPrefix = "setActiveToken";
std::string NetworkProtocolHelper::m_moveCursorPrefix = "moveCursor";
std::string NetworkProtocolHelper::m_endOfMessageToken = "<EOM>";
std::string NetworkProtocolHelper::m_createProjectPrefix = "createProject";
std::string NetworkProtocolHelper::m_createCDBProjectPrefix = "createCDBProject";
std::string NetworkProtocolHelper::m_createCDBPrefix = "createCDB";
std::string NetworkProtocolHelper::m_pingPrefix = "ping";

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
		else if (subMessages[0] == m_createCDBProjectPrefix)
		{
			return MESSAGE_TYPE::CREATE_CDB_MESSAGE;
		}
		else if (subMessages[0] == m_pingPrefix)
		{
			return MESSAGE_TYPE::PING;
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
			LOG_ERROR_STREAM(<< "Failed to parse message, invalid type token: " << subMessages[0] << ". Expected " << m_setActiveTokenPrefix);
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

					std::string nonConstId = ideId;
					boost::algorithm::to_lower(nonConstId);

					networkMessage.ideId = nonConstId;

					networkMessage.valid = true;
				}
				else
				{
					LOG_WARNING_STREAM(<< "Failed to parse ide ID string. Is " << ideId);
				}
			}
		}
		else
		{
			LOG_ERROR_STREAM(<< "Failed to parse message, invalid type token: " << subMessages[0] << ". Expected " << m_createProjectPrefix);
		}
	}

	return networkMessage;
}

NetworkProtocolHelper::CreateCDBProjectMessage NetworkProtocolHelper::parseCreateCDBProjectMessage(const std::string& message)
{
	std::vector<std::string> subMessages = divideMessage(message);

	NetworkProtocolHelper::CreateCDBProjectMessage networkMessage;

	if (subMessages.size() > 0)
	{
		if (subMessages[0] == m_createCDBProjectPrefix)
		{
			if (subMessages.size() < 4)
			{
				LOG_ERROR_STREAM(<< "Failed to parse createCDBProject message, too few tokens");
			}
			else
			{
				int subMessageCount = subMessages.size();

				std::string cdbPath = subMessages[1];
				if (cdbPath.length() > 0)
				{
					networkMessage.cdbFileLocation = cdbPath;
				}
				else
				{
					LOG_WARNING_STREAM(<< "CDB file path is not set.");
				}

				std::vector<std::string> headerPaths;
				for (int i = 2; i < subMessageCount - 2; i++)
				{
					if (subMessages[i].length() > 0)
					{
						headerPaths.push_back(subMessages[i]);
					}
				}
				networkMessage.headerPaths = headerPaths;

				std::string ideId = subMessages[subMessageCount - 2];
				if (ideId.length() > 0)
				{
					std::string nonConstId = ideId;
					boost::algorithm::to_lower(nonConstId);

					networkMessage.ideId = nonConstId;
				}
				else
				{
					LOG_WARNING_STREAM(<< "Failed to parse ide ID string. Is " << ideId);
				}

				if (networkMessage.cdbFileLocation.length() > 0 && networkMessage.ideId.length() > 0)
				{
					networkMessage.valid = true;
				}
			}
		}
		else
		{
			LOG_ERROR_STREAM(<< "Failed to parse message, invalid type token: " << subMessages[0] << ". Expected " << m_createCDBProjectPrefix);
		}
	}

	return networkMessage;
}

NetworkProtocolHelper::PingMessage NetworkProtocolHelper::parsePingMessage(const std::string& message)
{
	std::vector<std::string> subMessages = divideMessage(message);

	NetworkProtocolHelper::PingMessage pingMessage;

	if (subMessages.size() > 0)
	{
		if (subMessages[0] == m_pingPrefix)
		{
			if (subMessages.size() < 2)
			{
				LOG_ERROR_STREAM(<< "Failed to parse PingMessage message, too few tokens");
			}
			else
			{
				std::string ideId = subMessages[1];

				if (ideId.length() > 0)
				{
					std::string nonConstId = ideId;
					boost::algorithm::to_lower(nonConstId);

					pingMessage.ideId = ideId;

					pingMessage.valid = true;
				}
				else
				{
					LOG_WARNING_STREAM(<< "Failed to parse ide ID string. Is " << ideId);
				}
			}
		}
		else
		{
			LOG_ERROR_STREAM(<< "Failed to parse message, invalid type token: " << subMessages[0] << ". Expected " << m_pingPrefix);
		}
	}

	return pingMessage;
}

std::string NetworkProtocolHelper::buildSetIDECursorMessage(const std::string& fileLocation, const unsigned int row, const unsigned int column)
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

std::string NetworkProtocolHelper::buildCreateCDBMessage()
{
	std::stringstream messageStream;

	messageStream << m_createCDBPrefix;
	messageStream << m_endOfMessageToken;

	return messageStream.str();
}

std::string NetworkProtocolHelper::buildPingMessage()
{
	std::stringstream messageStream;

	messageStream << m_pingPrefix;
	messageStream << m_divider;
	messageStream << "coati";
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
