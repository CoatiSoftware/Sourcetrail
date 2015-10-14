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
		int row = 1;
		int column = 2;

		// valid message
		std::stringstream message;
		message << type << divider << filePath << divider << row << divider << column;

		NetworkProtocolHelper::NetworkMessage networkMessage = NetworkProtocolHelper::parseMessage(message.str());

		TS_ASSERT_EQUALS(networkMessage.fileLocation, filePath);
		TS_ASSERT_EQUALS(networkMessage.row, row);
		TS_ASSERT_EQUALS(networkMessage.column, column);

		// invalid type
		message.str("");
		message << "foo" << divider << filePath << divider << row << divider << column;
		networkMessage = NetworkProtocolHelper::parseMessage(message.str());

		TS_ASSERT_EQUALS(networkMessage.fileLocation, "");
		TS_ASSERT_EQUALS(networkMessage.row, 0);
		TS_ASSERT_EQUALS(networkMessage.column, 0);

		// missing divider
		message.str("");
		message << type << divider << filePath << row << divider << column;
		networkMessage = NetworkProtocolHelper::parseMessage(message.str());

		TS_ASSERT_EQUALS(networkMessage.fileLocation, "");
		TS_ASSERT_EQUALS(networkMessage.row, 0);
		TS_ASSERT_EQUALS(networkMessage.column, 0);

		// invalid row
		message.str("");
		message << type << divider << filePath << divider << "potato" << divider << column;
		networkMessage = NetworkProtocolHelper::parseMessage(message.str());

		TS_ASSERT_EQUALS(networkMessage.fileLocation, "");
		TS_ASSERT_EQUALS(networkMessage.row, 0);
		TS_ASSERT_EQUALS(networkMessage.column, 0);

		// invalid column
		message.str("");
		message << type << divider << filePath << divider << row << divider << "laz0r";
		networkMessage = NetworkProtocolHelper::parseMessage(message.str());

		TS_ASSERT_EQUALS(networkMessage.fileLocation, "");
		TS_ASSERT_EQUALS(networkMessage.row, 0);
		TS_ASSERT_EQUALS(networkMessage.column, 0);
	}
};