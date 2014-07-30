#ifndef PARSER_CLIENT_H
#define PARSER_CLIENT_H

#include <string>
#include <vector>

struct ParseLocation;
struct ParseTypeUsage;
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

	static std::string addAccessPrefix(const std::string& str, AccessType access);
	static std::string addAbstractionPrefix(const std::string& str, AbstractionType abstraction);
	static std::string addStaticPrefix(const std::string& str, bool isStatic);
	static std::string addConstPrefix(const std::string& str, bool isConst, bool atFront);
	static std::string addLocationSuffix(const std::string& str, const ParseLocation& location);
	static std::string addLocationSuffix(
		const std::string& str, const ParseLocation& location, const ParseLocation& scopeLocation);

	static std::string variableStr(const ParseVariable& variable);
	static std::string parameterStr(const std::vector<ParseTypeUsage> parameters);
	static std::string functionStr(
		const DataType& returnType,
		const std::string& fullName,
		const std::vector<ParseTypeUsage>& parameters,
		bool isConst
	);
	static std::string functionSignatureStr(
		const DataType& returnType,
		const std::string& fullName,
		const std::vector<ParseTypeUsage>& parameters,
		bool isConst
	);

	ParserClient();
	virtual ~ParserClient();

	virtual void onTypedefParsed(
		const ParseLocation& location, const std::string& fullName, const ParseTypeUsage& underlyingType,
		AccessType access) = 0;
	virtual void onClassParsed(
		const ParseLocation& location, const std::string& fullName, AccessType access,
		const ParseLocation& scopeLocation) = 0;
	virtual void onStructParsed(
		const ParseLocation& location, const std::string& fullName, AccessType access,
		const ParseLocation& scopeLocation) = 0;

	virtual void onGlobalVariableParsed(const ParseLocation& location, const ParseVariable& variable) = 0;
	virtual void onFieldParsed(const ParseLocation& location, const ParseVariable& variable, AccessType access) = 0;

	virtual void onFunctionParsed(
		const ParseLocation& location, const std::string& fullName, const ParseTypeUsage& returnType,
		const std::vector<ParseTypeUsage>& parameters, const ParseLocation& scopeLocation) = 0;
	virtual void onMethodParsed(
		const ParseLocation& location, const std::string& fullName, const ParseTypeUsage& returnType,
		const std::vector<ParseTypeUsage>& parameters, AccessType access, AbstractionType abstraction,
		bool isConst, bool isStatic, const ParseLocation& scopeLocation) = 0;

	virtual void onNamespaceParsed(
		const ParseLocation& location, const std::string& fullName, const ParseLocation& scopeLocation) = 0;

	virtual void onEnumParsed(
		const ParseLocation& location, const std::string& fullName, AccessType access,
		const ParseLocation& scopeLocation) = 0;
	virtual void onEnumFieldParsed(const ParseLocation& location, const std::string& fullName) = 0;

	virtual void onInheritanceParsed(
		const ParseLocation& location, const std::string& fullName, const std::string& baseName, AccessType access) = 0;
	virtual void onCallParsed(
		const ParseLocation& location, const std::string& callerName, const std::string& calleeName) = 0;
	virtual void onFieldUsageParsed(
		const ParseLocation& location, const std::string& userName, const std::string& usedName) = 0;
	virtual void onGlobalVariableUsageParsed(
		const ParseLocation& location, const std::string& userName, const std::string& usedName) = 0;
};

#endif // PARSER_CLIENT_H
