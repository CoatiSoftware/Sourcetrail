#ifndef PARSER_CLIENT_H
#define PARSER_CLIENT_H

#include <string>
#include <vector>

struct ParseLocation;
struct ParseVariable;
class DataType;

class ParserClient
{
public:
	enum AccessType {
		ACCESS_PUBLIC,
		ACCESS_PROTECTED,
		ACCESS_PRIVATE,
		ACCESS_NONE
	};

	enum AbstractionType {
		ABSTRACTION_VIRTUAL,
		ABSTRACTION_PURE_VIRTUAL,
		ABSTRACTION_NONE
	};

	ParserClient();
	virtual ~ParserClient();

	virtual void onTypedefParsed(
		const ParseLocation& location, const std::string& fullName, const DataType& underlyingType,
		AccessType access) = 0;
	virtual void onClassParsed(const ParseLocation& location, const std::string& fullName, AccessType access) = 0;
	virtual void onStructParsed(const ParseLocation& location, const std::string& fullName, AccessType access) = 0;

	virtual void onGlobalVariableParsed(const ParseLocation& location, const ParseVariable& variable) = 0;
	virtual void onFieldParsed(const ParseLocation& location, const ParseVariable& variable, AccessType access) = 0;

	virtual void onFunctionParsed(
		const ParseLocation& location, const std::string& fullName, const DataType& returnType,
		const std::vector<ParseVariable>& parameters) = 0;
	virtual void onMethodParsed(
		const ParseLocation& location, const std::string& fullName, const DataType& returnType,
		const std::vector<ParseVariable>& parameters, AccessType access, AbstractionType abstraction,
		bool isConst, bool isStatic) = 0;

	virtual void onNamespaceParsed(const ParseLocation& location, const std::string& fullName) = 0;

	virtual void onEnumParsed(const ParseLocation& location, const std::string& fullName, AccessType access) = 0;
	virtual void onEnumFieldParsed(const ParseLocation& location, const std::string& fullName) = 0;

	virtual void onInheritanceParsed(
		const ParseLocation& location, const std::string& fullName, const std::string& baseName, AccessType access) = 0;
	virtual void onCallParsed(
		const ParseLocation& location, const std::string& callerName, const std::string& calleeName) = 0;
};

#endif // PARSER_CLIENT_H
