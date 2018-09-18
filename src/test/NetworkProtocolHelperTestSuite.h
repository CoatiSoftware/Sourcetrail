#include <cxxtest/TestSuite.h>

#include "NetworkProtocolHelper.h"

class NetworkProtocolHelperTestSuite : public CxxTest::TestSuite
{
public:
	void test_parse_message(void)
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

		TS_ASSERT_EQUALS(networkMessage.filePath.wstr(), filePath);
		TS_ASSERT_EQUALS(networkMessage.row, row);
		TS_ASSERT_EQUALS(networkMessage.column, column);
		TS_ASSERT_EQUALS(networkMessage.valid, true);

		// invalid type
		message.str(L"");
		message << L"foo" << divider << filePath << divider << row << divider << column << endOfMessageToken;
		networkMessage = NetworkProtocolHelper::parseSetActiveTokenMessage(message.str());

		TS_ASSERT_EQUALS(networkMessage.filePath.wstr(), L"");
		TS_ASSERT_EQUALS(networkMessage.row, 0);
		TS_ASSERT_EQUALS(networkMessage.column, 0);
		TS_ASSERT_EQUALS(networkMessage.valid, false);

		// missing divider
		message.str(L"");
		message << type << divider << filePath << row << divider << column << endOfMessageToken;
		networkMessage = NetworkProtocolHelper::parseSetActiveTokenMessage(message.str());

		TS_ASSERT_EQUALS(networkMessage.filePath.wstr(), L"");
		TS_ASSERT_EQUALS(networkMessage.row, 0);
		TS_ASSERT_EQUALS(networkMessage.column, 0);
		TS_ASSERT_EQUALS(networkMessage.valid, false);

		// invalid row
		message.str(L"");
		message << type << divider << filePath << divider << "potato" << divider << column << endOfMessageToken;
		networkMessage = NetworkProtocolHelper::parseSetActiveTokenMessage(message.str());

		TS_ASSERT_EQUALS(networkMessage.filePath.wstr(), L"");
		TS_ASSERT_EQUALS(networkMessage.row, 0);
		TS_ASSERT_EQUALS(networkMessage.column, 0);
		TS_ASSERT_EQUALS(networkMessage.valid, false);

		// invalid column
		message.str(L"");
		message << type << divider << filePath << divider << row << divider << "laz0r" << endOfMessageToken;
		networkMessage = NetworkProtocolHelper::parseSetActiveTokenMessage(message.str());

		TS_ASSERT_EQUALS(networkMessage.filePath.wstr(), L"");
		TS_ASSERT_EQUALS(networkMessage.row, 0);
		TS_ASSERT_EQUALS(networkMessage.column, 0);
		TS_ASSERT_EQUALS(networkMessage.valid, false);
	}
};