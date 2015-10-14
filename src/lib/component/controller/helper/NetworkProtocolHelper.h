#ifndef NETWORK_PROTOCOL_HELPER_H
#define NETWORK_PROTOCOL_HELPER_H

#include <string>

class NetworkProtocolHelper
{
public:
	struct NetworkMessage
	{
	public:
		NetworkMessage()
			: fileLocation("")
			, row(0)
			, column(0)
			, valid(false)
		{}

		std::string fileLocation;
		unsigned int row;
		unsigned int column;
		bool valid;
	};

	static NetworkMessage parseMessage(const std::string& message);
	static std::string buildMessage(const std::string& fileLocation, const unsigned int row, const unsigned int column);

private:
	static std::string removeEndOfMessageToken(const std::string& message);

	static std::string getSubstringAfterString(const std::string& message, const std::string& searchString, std::string& subMessage);
	static bool isDigits(const std::string& text);

	static std::string m_divider;
	static std::string m_setActiveTokenPrefix;
	static std::string m_moveCursorPrefix;
	static std::string m_endOfMessageToken;
};

#endif // NETWORK_PROTOCOL_HELPER_H