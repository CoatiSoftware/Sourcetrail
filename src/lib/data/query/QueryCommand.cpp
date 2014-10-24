#include "data/query/QueryCommand.h"

#include <algorithm>

std::map<std::string, QueryCommand::CommandType> QueryCommand::getCommandTypeMap()
{
	static std::map<std::string, CommandType> commandMap;

	if (commandMap.size())
	{
		return commandMap;
	}

	commandMap.emplace("member", COMMAND_MEMBER);
	commandMap.emplace("child", COMMAND_MEMBER);

	commandMap.emplace("parent", COMMAND_PARENT);

	commandMap.emplace("function", COMMAND_FUNCTION);
	commandMap.emplace("variable", COMMAND_GLOBAL_VARIABLE);
	commandMap.emplace("global", COMMAND_GLOBAL_VARIABLE);
	commandMap.emplace("class", COMMAND_CLASS);
	commandMap.emplace("method", COMMAND_METHOD);
	commandMap.emplace("field", COMMAND_FIELD);
	commandMap.emplace("namespace", COMMAND_NAMESPACE);
	commandMap.emplace("struct", COMMAND_STRUCT);
	commandMap.emplace("enum", COMMAND_ENUM);
	commandMap.emplace("typedef", COMMAND_TYPEDEF);

	commandMap.emplace("const", COMMAND_CONST);
	commandMap.emplace("static", COMMAND_STATIC);
	commandMap.emplace("virtual", COMMAND_VIRTUAL);
	commandMap.emplace("abstract", COMMAND_PURE_VIRTUAL);
	commandMap.emplace("pure-virtual", COMMAND_PURE_VIRTUAL);

	commandMap.emplace("public", COMMAND_PUBLIC);
	commandMap.emplace("protected", COMMAND_PROTECTED);
	commandMap.emplace("private", COMMAND_PRIVATE);

	commandMap.emplace("caller", COMMAND_CALLER);
	commandMap.emplace("callee", COMMAND_CALLEE);

	commandMap.emplace("usage", COMMAND_USAGE);

	commandMap.emplace("base", COMMAND_SUPER_CLASS);
	commandMap.emplace("super", COMMAND_SUPER_CLASS);
	commandMap.emplace("derived", COMMAND_SUB_CLASS);
	commandMap.emplace("subclass", COMMAND_SUB_CLASS);

	return commandMap;
}

QueryCommand::QueryCommand(std::string name)
	: m_type(COMMAND_INVALID)
{
	name.erase(std::remove(name.begin(), name.end(), BOUNDARY), name.end());
	m_name = name;

	std::map<std::string, CommandType> commandMap = getCommandTypeMap();
	std::map<std::string, CommandType>::iterator it = commandMap.find(name);

	if (it != commandMap.end())
	{
		m_type = it->second;
	}
}

QueryCommand::~QueryCommand()
{
}

bool QueryCommand::isCommand() const
{
	return true;
}

bool QueryCommand::isOperator() const
{
	return false;
}

bool QueryCommand::isToken() const
{
	return false;
}

bool QueryCommand::derivedIsComplete() const
{
	return m_type != COMMAND_INVALID;
}

std::string QueryCommand::getName() const
{
	return m_name;
}

void QueryCommand::print(std::ostream& ostream) const
{
	ostream << BOUNDARY << m_name << BOUNDARY;
}

QueryCommand::CommandType QueryCommand::getType() const
{
	return m_type;
}

const char QueryCommand::BOUNDARY = '\'';
