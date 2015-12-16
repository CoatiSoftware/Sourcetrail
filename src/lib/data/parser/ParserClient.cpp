#include "data/parser/ParserClient.h"

#include <sstream>

#include "data/parser/ParseFunction.h"
#include "data/parser/ParseLocation.h"
#include "data/parser/ParseTypeUsage.h"
#include "data/parser/ParseVariable.h"
#include "data/type/DataType.h"
#include "utility/utilityString.h"

std::string ParserClient::addAccessPrefix(const std::string& str, AccessType access)
{
	switch (access)
	{
	case ACCESS_PUBLIC:
		return "public " + str;
	case ACCESS_PROTECTED:
		return "protected " + str;
	case ACCESS_PRIVATE:
		return "private " + str;
	case ACCESS_NONE:
		return str;
	}
}

std::string ParserClient::addAbstractionPrefix(const std::string& str, AbstractionType abstraction)
{
	switch (abstraction)
	{
	case ABSTRACTION_VIRTUAL:
		return "virtual " + str;
	case ABSTRACTION_PURE_VIRTUAL:
		return "pure virtual " + str;
	case ABSTRACTION_NONE:
		return str;
	}
}

std::string ParserClient::addStaticPrefix(const std::string& str, bool isStatic)
{
	if (isStatic)
	{
		return "static " + str;
	}
	return str;
}

std::string ParserClient::addConstPrefix(const std::string& str, bool isConst, bool atFront)
{
	if (isConst)
	{
		return atFront ? "const " + str : str + " const";
	}
	return str;
}

std::string ParserClient::addLocationSuffix(const std::string& str, const ParseLocation& location)
{
	std::stringstream ss;
	ss << str;
	ss << " <" << location.startLineNumber << ":" << location.startColumnNumber << " ";
	ss << location.endLineNumber << ":" << location.endColumnNumber << ">";
	return ss.str();
}

std::string ParserClient::addLocationSuffix(
	const std::string& str, const ParseLocation& location, const ParseLocation& scopeLocation
){
	if (!location.isValid())
	{
		return addLocationSuffix(str, scopeLocation);
	}
	else if (!scopeLocation.isValid())
	{
		return addLocationSuffix(str, location);
	}

	std::stringstream ss;
	ss << str;
	ss << " <" << scopeLocation.startLineNumber << ":" << scopeLocation.startColumnNumber;
	ss << " <" << location.startLineNumber << ":" << location.startColumnNumber << " ";
	ss << location.endLineNumber << ":" << location.endColumnNumber << "> ";
	ss << scopeLocation.endLineNumber << ":" << scopeLocation.endColumnNumber << ">";
	return ss.str();
}

std::string ParserClient::variableStr(const ParseVariable& variable)
{
	std::string str = variable.type.dataType->getFullTypeName() + " " + variable.getFullName();
	return addStaticPrefix(str, variable.isStatic);
}

std::string ParserClient::parameterStr(const std::vector<ParseTypeUsage> parameters)
{
	std::string str = "(";
	for (size_t i = 0; i < parameters.size(); i++)
	{
		str += parameters[i].dataType->getFullTypeName();
		if (i < parameters.size() - 1)
		{
			str += ", ";
		}
	}
	return str + ")";
}

std::string ParserClient::functionStr(const ParseFunction& function)
{
	std::string str =
		function.returnType.dataType->getFullTypeName() + " " + function.getFullName() + parameterStr(function.parameters);
	return addConstPrefix(addStaticPrefix(str, function.isStatic), function.isConst, false);
}

ParserClient::ParserClient()
{
}

ParserClient::~ParserClient()
{
}
