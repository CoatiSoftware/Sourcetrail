#ifndef QUERY_COMMAND_H
#define QUERY_COMMAND_H

#include <map>

#include "data/query/QueryNode.h"

class QueryCommand
	: public QueryNode
{
public:
	enum CommandType
	{
		COMMAND_INVALID,

		COMMAND_MEMBER,
		COMMAND_PARENT,
		COMMAND_FUNCTION,
		COMMAND_GLOBAL_VARIABLE,
		COMMAND_CLASS,
		COMMAND_METHOD,
		COMMAND_FIELD,
		COMMAND_NAMESPACE,
		COMMAND_STRUCT,
		COMMAND_ENUM,
		COMMAND_ENUM_CONSTANT,
		COMMAND_TYPEDEF,

		COMMAND_CONST,
		COMMAND_STATIC,
		COMMAND_VIRTUAL,
		COMMAND_PURE_VIRTUAL,

		COMMAND_PUBLIC,
		COMMAND_PROTECTED,
		COMMAND_PRIVATE,

		COMMAND_CALLER,
		COMMAND_CALLEE,

		COMMAND_USAGE,

		COMMAND_SUPER_CLASS,
		COMMAND_SUB_CLASS
	};

	static std::map<std::string, CommandType> getCommandTypeMap();

	QueryCommand(std::string name);
	~QueryCommand();

	virtual bool isCommand() const;
	virtual bool isOperator() const;
	virtual bool isToken() const;

	virtual bool derivedIsComplete() const;

	virtual std::string getName() const;

	virtual void print(std::ostream& ostream) const;

	CommandType getType() const;

	static const char BOUNDARY;

private:
	CommandType m_type;
	std::string m_name;
};

#endif // QUERY_COMMAND_H
