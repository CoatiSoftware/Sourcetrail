#include "NetworkProtocolHelper.h"

#include <sstream>
#include <string>

#include <boost/algorithm/string.hpp>

#include "logging.h"
#include "utilityString.h"

std::wstring NetworkProtocolHelper::s_divider = L">>";
std::wstring NetworkProtocolHelper::s_setActiveTokenPrefix = L"setActiveToken";
std::wstring NetworkProtocolHelper::s_moveCursorPrefix = L"moveCursor";
std::wstring NetworkProtocolHelper::s_endOfMessageToken = L"<EOM>";
std::wstring NetworkProtocolHelper::s_createProjectPrefix = L"createProject";
std::wstring NetworkProtocolHelper::s_createCDBProjectPrefix = L"createCDBProject";
std::wstring NetworkProtocolHelper::s_createCDBPrefix = L"createCDB";
std::wstring NetworkProtocolHelper::s_pingPrefix = L"ping";

NetworkProtocolHelper::MESSAGE_TYPE NetworkProtocolHelper::getMessageType(const std::wstring& message)
{
	std::vector<std::wstring> subMessages = divideMessage(message);

	if (!subMessages.empty())
	{
		if (subMessages[0] == s_setActiveTokenPrefix)
		{
			return MESSAGE_TYPE::SET_ACTIVE_TOKEN;
		}
		else if (subMessages[0] == s_createProjectPrefix)
		{
			return MESSAGE_TYPE::CREATE_PROJECT;
		}
		else if (subMessages[0] == s_createCDBProjectPrefix)
		{
			return MESSAGE_TYPE::CREATE_CDB_MESSAGE;
		}
		else if (subMessages[0] == s_pingPrefix)
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

NetworkProtocolHelper::SetActiveTokenMessage NetworkProtocolHelper::parseSetActiveTokenMessage(
	const std::wstring& message)
{
	std::vector<std::wstring> subMessages = divideMessage(message);

	SetActiveTokenMessage networkMessage;

	if (!subMessages.empty())
	{
		if (subMessages[0] == s_setActiveTokenPrefix)
		{
			if (subMessages.size() != 5)
			{
				LOG_ERROR("Failed to parse setActiveToken message, invalid token count");
			}
			else
			{
				const std::wstring filePath = subMessages[1];
				const std::wstring row = subMessages[2];
				const std::wstring column = subMessages[3];

				if (!filePath.empty() && !row.empty() && !column.empty() && isDigits(row) &&
					isDigits(column))
				{
					networkMessage.filePath = FilePath(filePath);
					networkMessage.row = std::stoi(row);
					networkMessage.column = std::stoi(column);
					networkMessage.valid = true;
				}
			}
		}
		else
		{
			LOG_ERROR(
				L"Failed to parse message, invalid type token: " + subMessages[0] + L". Expected " +
				s_setActiveTokenPrefix);
		}
	}

	return networkMessage;
}

NetworkProtocolHelper::CreateProjectMessage NetworkProtocolHelper::parseCreateProjectMessage(
	const std::wstring& message)
{
	std::vector<std::wstring> subMessages = divideMessage(message);

	NetworkProtocolHelper::CreateProjectMessage networkMessage;

	if (!subMessages.empty())
	{
		if (subMessages[0] == s_createProjectPrefix)
		{
			if (subMessages.size() != 4)
			{
				LOG_ERROR("Failed to parse createProject message, invalid token count");
			}
		}
		else
		{
			LOG_ERROR(
				L"Failed to parse message, invalid type token: " + subMessages[0] + L". Expected " +
				s_createProjectPrefix);
		}
	}

	return networkMessage;
}

NetworkProtocolHelper::CreateCDBProjectMessage NetworkProtocolHelper::parseCreateCDBProjectMessage(
	const std::wstring& message)
{
	std::vector<std::wstring> subMessages = divideMessage(message);

	NetworkProtocolHelper::CreateCDBProjectMessage networkMessage;

	if (!subMessages.empty())
	{
		if (subMessages[0] == s_createCDBProjectPrefix)
		{
			if (subMessages.size() < 4)
			{
				LOG_ERROR("Failed to parse createCDBProject message, too few tokens");
			}
			else
			{
				const size_t subMessageCount = subMessages.size();

				const std::wstring cdbPath = subMessages[1];
				if (!cdbPath.empty())
				{
					networkMessage.cdbFileLocation = FilePath(cdbPath);
				}
				else
				{
					LOG_WARNING("CDB file path is not set.");
				}

				const std::wstring ideId = subMessages[subMessageCount - 2];
				if (!ideId.empty())
				{
					std::wstring nonConstId = ideId;
					boost::algorithm::to_lower(nonConstId);

					networkMessage.ideId = nonConstId;
				}
				else
				{
					LOG_WARNING(L"Failed to parse ide ID string. Is " + ideId);
				}

				if (!networkMessage.cdbFileLocation.empty() && !networkMessage.ideId.empty())
				{
					networkMessage.valid = true;
				}
			}
		}
		else
		{
			LOG_ERROR(
				L"Failed to parse message, invalid type token: " + subMessages[0] + L". Expected " +
				s_createCDBProjectPrefix);
		}
	}

	return networkMessage;
}

NetworkProtocolHelper::PingMessage NetworkProtocolHelper::parsePingMessage(const std::wstring& message)
{
	std::vector<std::wstring> subMessages = divideMessage(message);

	NetworkProtocolHelper::PingMessage pingMessage;

	if (!subMessages.empty())
	{
		if (subMessages[0] == s_pingPrefix)
		{
			if (subMessages.size() < 2)
			{
				LOG_ERROR_STREAM(<< "Failed to parse PingMessage message, too few tokens");
			}
			else
			{
				std::wstring ideId = subMessages[1];

				if (!ideId.empty())
				{
					std::wstring nonConstId = ideId;
					boost::algorithm::to_lower(nonConstId);

					pingMessage.ideId = ideId;

					pingMessage.valid = true;
				}
				else
				{
					LOG_WARNING(L"Failed to parse ide ID string: " + ideId);
				}
			}
		}
		else
		{
			LOG_ERROR(
				L"Failed to parse message, invalid type token: " + subMessages[0] + L". Expected " +
				s_pingPrefix);
		}
	}

	return pingMessage;
}

std::wstring NetworkProtocolHelper::buildSetIDECursorMessage(
	const FilePath& fileLocation, const unsigned int row, const unsigned int column)
{
	std::wstringstream messageStream;

	messageStream << s_moveCursorPrefix;
	messageStream << s_divider;
	messageStream << fileLocation.wstr();
	messageStream << s_divider;
	messageStream << row;
	messageStream << s_divider;
	messageStream << column;
	messageStream << s_endOfMessageToken;

	return messageStream.str();
}

std::wstring NetworkProtocolHelper::buildCreateCDBMessage()
{
	std::wstringstream messageStream;

	messageStream << s_createCDBPrefix;
	messageStream << s_endOfMessageToken;

	return messageStream.str();
}

std::wstring NetworkProtocolHelper::buildPingMessage()
{
	std::wstringstream messageStream;

	messageStream << s_pingPrefix;
	messageStream << s_divider;
	messageStream << "sourcetrail";
	messageStream << s_endOfMessageToken;

	return messageStream.str();
}

std::vector<std::wstring> NetworkProtocolHelper::divideMessage(const std::wstring& message)
{
	std::vector<std::wstring> result;

	std::wstring msg = message;
	size_t pos = msg.find(s_divider);

	while (pos != std::wstring::npos)
	{
		std::wstring subMessage = msg.substr(0, pos);
		result.push_back(subMessage);
		msg = msg.substr(pos + s_divider.size());
		pos = msg.find(s_divider);
	}

	if (msg.size() > 0)
	{
		pos = msg.find(s_endOfMessageToken);
		if (pos != std::wstring::npos)
		{
			std::wstring subMessage = msg.substr(0, pos);
			result.push_back(subMessage);
			msg = msg.substr(pos);
			result.push_back(msg);
		}
	}

	return result;
}

bool NetworkProtocolHelper::isDigits(const std::wstring& text)
{
	return (text.find_first_not_of(L"0123456789") == std::wstring::npos);
}
