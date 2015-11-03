#include <cxxtest/TestSuite.h>

#include "component/controller/helper/NetworkProtocolHelper.h"

class NetworkProtocolHelperTestSuite : public CxxTest::TestSuite
{
public:
	void test_parse_message(void)
	{
		std::string type = "setActiveToken";
		std::string divider = ">>";
		std::string filePath = "C:\\Users\\Manuel\\imporant\\file\\location\\fileName.cpp";
		std::string endOfMessageToken = "<EOM>";
		int row = 1;
		int column = 2;

		// valid message
		std::stringstream message;
		message << type << divider << filePath << divider << row << divider << column << endOfMessageToken;

		NetworkProtocolHelper::NetworkMessage networkMessage = NetworkProtocolHelper::parseMessage(message.str());

		TS_ASSERT_EQUALS(networkMessage.fileLocation, filePath);
		TS_ASSERT_EQUALS(networkMessage.row, row);
		TS_ASSERT_EQUALS(networkMessage.column, column);
		TS_ASSERT_EQUALS(networkMessage.valid, true);

		// invalid type
		message.str("");
		message << "foo" << divider << filePath << divider << row << divider << column << endOfMessageToken;
		networkMessage = NetworkProtocolHelper::parseMessage(message.str());

		TS_ASSERT_EQUALS(networkMessage.fileLocation, "");
		TS_ASSERT_EQUALS(networkMessage.row, 0);
		TS_ASSERT_EQUALS(networkMessage.column, 0);
		TS_ASSERT_EQUALS(networkMessage.valid, false);

		// missing divider
		message.str("");
		message << type << divider << filePath << row << divider << column << endOfMessageToken;
		networkMessage = NetworkProtocolHelper::parseMessage(message.str());

		TS_ASSERT_EQUALS(networkMessage.fileLocation, "");
		TS_ASSERT_EQUALS(networkMessage.row, 0);
		TS_ASSERT_EQUALS(networkMessage.column, 0);
		TS_ASSERT_EQUALS(networkMessage.valid, false);

		// invalid row
		message.str("");
		message << type << divider << filePath << divider << "potato" << divider << column << endOfMessageToken;
		networkMessage = NetworkProtocolHelper::parseMessage(message.str());

		TS_ASSERT_EQUALS(networkMessage.fileLocation, "");
		TS_ASSERT_EQUALS(networkMessage.row, 0);
		TS_ASSERT_EQUALS(networkMessage.column, 0);
		TS_ASSERT_EQUALS(networkMessage.valid, false);

		// invalid column
		message.str("");
		message << type << divider << filePath << divider << row << divider << "laz0r" << endOfMessageToken;
		networkMessage = NetworkProtocolHelper::parseMessage(message.str());

		TS_ASSERT_EQUALS(networkMessage.fileLocation, "");
		TS_ASSERT_EQUALS(networkMessage.row, 0);
		TS_ASSERT_EQUALS(networkMessage.column, 0);
		TS_ASSERT_EQUALS(networkMessage.valid, false);
	}
};