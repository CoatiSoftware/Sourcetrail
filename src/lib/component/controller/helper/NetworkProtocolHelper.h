#ifndef NETWORK_PROTOCOL_HELPER_H
#define NETWORK_PROTOCOL_HELPER_H

#include <string>
#include <vector>

class NetworkProtocolHelper
{
public:
	struct SetActiveTokenMessage
	{
	public:
		SetActiveTokenMessage()
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

	struct CreateProjectMessage
	{
	public:
		CreateProjectMessage()
			: solutionFileLocation("")
			, ideId("")
			, valid(false)
		{}

		std::string solutionFileLocation;
		std::string ideId;
		bool valid;
	};

	struct CreateCDBProjectMessage
	{
	public:
		CreateCDBProjectMessage()
			: cdbFileLocation("")
			, headerPaths()
			, ideId("")
			, valid(false)
		{}

		std::string cdbFileLocation;
		std::vector<std::string> headerPaths;
		std::string ideId;
		bool valid;
	};

	enum MESSAGE_TYPE
	{
		UNKNOWN = 0,
		SET_ACTIVE_TOKEN,
		CREATE_PROJECT,
		CREATE_CDB_MESSAGE
	};

	static MESSAGE_TYPE getMessageType(const std::string& message);

	static SetActiveTokenMessage parseSetActiveTokenMessage(const std::string& message);
	static CreateProjectMessage parseCreateProjectMessage(const std::string& message);
	static CreateCDBProjectMessage parseCreateCDBProjectMessage(const std::string& message);

	static std::string buildSetIDECursorMessage(const std::string& fileLocation, const unsigned int row, const unsigned int column);
	static std::string buildCreateCDBMessage();

private:
	static std::vector<std::string> divideMessage(const std::string& message);

	static std::string removeEndOfMessageToken(const std::string& message);

	static std::string getSubstringAfterString(const std::string& message, const std::string& searchString, std::string& subMessage);
	static bool isDigits(const std::string& text);

	static std::string m_divider;
	static std::string m_setActiveTokenPrefix;
	static std::string m_moveCursorPrefix;
	static std::string m_endOfMessageToken;
	static std::string m_createProjectPrefix;
	static std::string m_createCDBProjectPrefix;
	static std::string m_createCDBPrefix;
};

#endif // NETWORK_PROTOCOL_HELPER_H