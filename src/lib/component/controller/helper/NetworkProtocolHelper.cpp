#include "NetworkProtocolHelper.h"

#include <sstream>
#include <string>

std::string NetworkProtocolHelper::m_divider = ">>";
std::string NetworkProtocolHelper::m_setActiveTokenPrefix = "setActiveToken";
std::string NetworkProtocolHelper::m_moveCursorPrefix = "moveCursor";
std::string NetworkProtocolHelper::m_endOfMessageToken = "<EOM>";

NetworkProtocolHelper::NetworkMessage NetworkProtocolHelper::parseMessage(const std::string& message)
{
	NetworkMessage networkMessage;
	
	size_t pos = message.find(m_setActiveTokenPrefix);

	if(pos != std::string::npos)
	{
		std::string tmpMessage = "";
		getSubstringAfterString(message, m_divider, tmpMessage); // just get rid of message type
		tmpMessage = removeEndOfMessageToken(tmpMessage);

		std::string fileLocation = getSubstringAfterString(tmpMessage, m_divider, tmpMessage);
		std::string row = getSubstringAfterString(tmpMessage, m_divider, tmpMessage);
		std::string column = tmpMessage; // there should be nothing left in the message but the column number

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
