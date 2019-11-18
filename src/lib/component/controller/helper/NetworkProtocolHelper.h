#ifndef NETWORK_PROTOCOL_HELPER_H
#define NETWORK_PROTOCOL_HELPER_H

#include <string>
#include <vector>

#include "FilePath.h"

class NetworkProtocolHelper
{
public:
	struct SetActiveTokenMessage
	{
	public:
		SetActiveTokenMessage(): filePath(L""), row(0), column(0), valid(false) {}

		FilePath filePath;
		unsigned int row;
		unsigned int column;
		bool valid;
	};

	struct CreateProjectMessage
	{
	};

	struct CreateCDBProjectMessage
	{
	public:
		CreateCDBProjectMessage(): cdbFileLocation(L""), ideId(L""), valid(false) {}

		FilePath cdbFileLocation;
		std::wstring ideId;
		bool valid;
	};

	struct PingMessage
	{
	public:
		PingMessage(): ideId(L""), valid(false) {}

		std::wstring ideId;
		bool valid;
	};

	enum MESSAGE_TYPE
	{
		UNKNOWN = 0,
		SET_ACTIVE_TOKEN,
		CREATE_PROJECT,
		CREATE_CDB_MESSAGE,
		PING
	};

	static MESSAGE_TYPE getMessageType(const std::wstring& message);

	static SetActiveTokenMessage parseSetActiveTokenMessage(const std::wstring& message);
	static CreateProjectMessage parseCreateProjectMessage(const std::wstring& message);
	static CreateCDBProjectMessage parseCreateCDBProjectMessage(const std::wstring& message);
	static PingMessage parsePingMessage(const std::wstring& message);

	static std::wstring buildSetIDECursorMessage(
		const FilePath& fileLocation, const unsigned int row, const unsigned int column);
	static std::wstring buildCreateCDBMessage();
	static std::wstring buildPingMessage();

private:
	static std::vector<std::wstring> divideMessage(const std::wstring& message);
	static bool isDigits(const std::wstring& text);

	static std::wstring s_divider;
	static std::wstring s_setActiveTokenPrefix;
	static std::wstring s_moveCursorPrefix;
	static std::wstring s_endOfMessageToken;
	static std::wstring s_createProjectPrefix;
	static std::wstring s_createCDBProjectPrefix;
	static std::wstring s_createCDBPrefix;
	static std::wstring s_pingPrefix;
};

#endif	  // NETWORK_PROTOCOL_HELPER_H