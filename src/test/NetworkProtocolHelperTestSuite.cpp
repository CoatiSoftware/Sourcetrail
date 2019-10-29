#include "catch.hpp"

#include <sstream>

#include "NetworkProtocolHelper.h"

TEST_CASE("parse message")
{
	std::wstring type = L"setActiveToken";
	std::wstring divider = L">>";
	std::wstring filePath = L"C:/Users/Manuel/imporant/file/location/fileName.cpp";
	std::wstring endOfMessageToken = L"<EOM>";
	int row = 1;
	int column = 2;

	// valid message
	std::wstringstream message;
	message << type << divider << filePath << divider << row << divider << column << endOfMessageToken;

	NetworkProtocolHelper::SetActiveTokenMessage networkMessage = NetworkProtocolHelper::parseSetActiveTokenMessage(message.str());

	REQUIRE(networkMessage.filePath.wstr() == filePath);
	REQUIRE(networkMessage.row == row);
	REQUIRE(networkMessage.column == column);
	REQUIRE(networkMessage.valid == true);

	// invalid type
	message.str(L"");
	message << L"foo" << divider << filePath << divider << row << divider << column << endOfMessageToken;
	networkMessage = NetworkProtocolHelper::parseSetActiveTokenMessage(message.str());

	REQUIRE(networkMessage.filePath.wstr() == L"");
	REQUIRE(networkMessage.row == 0);
	REQUIRE(networkMessage.column == 0);
	REQUIRE(networkMessage.valid == false);

	// missing divider
	message.str(L"");
	message << type << divider << filePath << row << divider << column << endOfMessageToken;
	networkMessage = NetworkProtocolHelper::parseSetActiveTokenMessage(message.str());

	REQUIRE(networkMessage.filePath.wstr() == L"");
	REQUIRE(networkMessage.row == 0);
	REQUIRE(networkMessage.column == 0);
	REQUIRE(networkMessage.valid == false);

	// invalid row
	message.str(L"");
	message << type << divider << filePath << divider << "potato" << divider << column << endOfMessageToken;
	networkMessage = NetworkProtocolHelper::parseSetActiveTokenMessage(message.str());

	REQUIRE(networkMessage.filePath.wstr() == L"");
	REQUIRE(networkMessage.row == 0);
	REQUIRE(networkMessage.column == 0);
	REQUIRE(networkMessage.valid == false);

	// invalid column
	message.str(L"");
	message << type << divider << filePath << divider << row << divider << "laz0r" << endOfMessageToken;
	networkMessage = NetworkProtocolHelper::parseSetActiveTokenMessage(message.str());

	REQUIRE(networkMessage.filePath.wstr() == L"");
	REQUIRE(networkMessage.row == 0);
	REQUIRE(networkMessage.column == 0);
	REQUIRE(networkMessage.valid == false);
}
