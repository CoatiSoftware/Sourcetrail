#include "data/Storage.h"

#include <sstream>

#include "data/parser/ParseLocation.h"
#include "data/parser/ParseVariable.h"
#include "utility/logging/logging.h"

Storage::Storage()
{
}

Storage::~Storage()
{
}

void Storage::onTypedefParsed(
	const ParseLocation& location, const std::string& fullName, const std::string& underlyingFullName,
	AccessType access
)
{
	log("typedef", fullName + " -> " + underlyingFullName, location);
}

void Storage::onClassParsed(const ParseLocation& location, const std::string& fullName, AccessType access)
{
	log("class", fullName, location);
}

void Storage::onStructParsed(const ParseLocation& location, const std::string& fullName, AccessType access)
{
	log("struct", fullName, location);
}

void Storage::onGlobalVariableParsed(const ParseLocation& location, const ParseVariable& variable)
{
	log("global", variable.fullName, location);
}

void Storage::onFieldParsed(const ParseLocation& location, const ParseVariable& variable, AccessType access)
{
	log("field", variable.fullName, location);
}

void Storage::onFunctionParsed(
	const ParseLocation& location, const std::string& fullName, const std::string& returnTypeName,
	const std::vector<ParseVariable>& parameters
)
{
	log("function", fullName, location);
}

void Storage::onMethodParsed(
	const ParseLocation& location, const std::string& fullName, const std::string& returnTypeName,
	const std::vector<ParseVariable>& parameters, AccessType access, AbstractionType abstraction,
	bool isConst, bool isStatic
)
{
	log("method", fullName, location);
}

void Storage::onNamespaceParsed(const ParseLocation& location, const std::string& fullName)
{
	log("namespace", fullName, location);
}

void Storage::onEnumParsed(const ParseLocation& location, const std::string& fullName, AccessType access)
{
	log("enum", fullName, location);
}

void Storage::onEnumFieldParsed(const ParseLocation& location, const std::string& fullName)
{
	log("enum field", fullName, location);
}

void Storage::log(std::string type, std::string str, const ParseLocation& location) const
{
	std::stringstream info;
	info << type << ": " << str << " <" << location.file << " " << location.line << ":" << location.column << ">";
	LOG_INFO(info.str());
}
