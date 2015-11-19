#ifndef PARSER_CLIENT_H
#define PARSER_CLIENT_H

#include <string>
#include <vector>

#include "utility/types.h"
#include "data/name/NameHierarchy.h"

#include "utility/file/FileInfo.h"

#include "ParseLocation.h"


struct ParseFunction;
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

	enum RecordType {
		RECORD_STRUCT,
		RECORD_CLASS
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
	static std::string functionStr(const ParseFunction& function);
	static std::string functionSignatureStr(const ParseFunction& function);

	ParserClient();
	virtual ~ParserClient();

	virtual void startParsing() = 0;
	virtual void finishParsing() = 0;

	virtual void prepareParsingFile(const FilePath& filePath) = 0;
	virtual void finishParsingFile(const FilePath& filePath) = 0;

	virtual void onError(const ParseLocation& location, const std::string& message) = 0;
	virtual size_t getErrorCount() const = 0;

	virtual Id onTypedefParsed(
		const ParseLocation& location, const NameHierarchy& nameHierarchy,
		const ParseTypeUsage& underlyingType, AccessType access) = 0;
	virtual Id onClassParsed(
		const ParseLocation& location, const NameHierarchy& nameHierarchy, AccessType access,
		const ParseLocation& scopeLocation) = 0;
	virtual Id onStructParsed(
		const ParseLocation& location, const NameHierarchy& nameHierarchy, AccessType access,
		const ParseLocation& scopeLocation) = 0;

	virtual Id onGlobalVariableParsed(const ParseLocation& location, const ParseVariable& variable) = 0;
	virtual Id onFieldParsed(const ParseLocation& location, const ParseVariable& variable, AccessType access) = 0;

	virtual Id onFunctionParsed(
		const ParseLocation& location, const ParseFunction& function, const ParseLocation& scopeLocation) = 0;
	virtual Id onMethodParsed(
		const ParseLocation& location, const ParseFunction& method, AccessType access, AbstractionType abstraction,
		const ParseLocation& scopeLocation) = 0;

	virtual Id onNamespaceParsed(
		const ParseLocation& location, const NameHierarchy& nameHierarchy,
		const ParseLocation& scopeLocation) = 0;

	virtual Id onEnumParsed(
		const ParseLocation& location, const NameHierarchy& nameHierarchy, AccessType access,
		const ParseLocation& scopeLocation) = 0;
	virtual Id onEnumConstantParsed(const ParseLocation& location, const NameHierarchy& nameHierarchy) = 0;

	virtual Id onInheritanceParsed(
		const ParseLocation& location, const NameHierarchy& nameHierarchy,
		const NameHierarchy& baseNameHierarchy, AccessType access) = 0;
	virtual Id onMethodOverrideParsed(
		const ParseLocation& location, const ParseFunction& base, const ParseFunction& overrider) = 0;
	virtual Id onCallParsed(
		const ParseLocation& location, const ParseFunction& caller, const ParseFunction& callee) = 0;
	virtual Id onCallParsed(
		const ParseLocation& location, const ParseVariable& caller, const ParseFunction& callee) = 0;
	virtual Id onFieldUsageParsed(
		const ParseLocation& location, const ParseFunction& user, const NameHierarchy& usedNameHierarchy) = 0;
	virtual Id onFieldUsageParsed(
		const ParseLocation& location, const ParseVariable& user, const NameHierarchy& usedNameHierarchy) = 0;
	virtual Id onGlobalVariableUsageParsed(
		const ParseLocation& location, const ParseFunction& user, const NameHierarchy& usedNameHierarchy) = 0;
	virtual Id onGlobalVariableUsageParsed(
		const ParseLocation& location, const ParseVariable& user, const NameHierarchy& usedNameHierarchy) = 0;
	virtual Id onEnumConstantUsageParsed(
		const ParseLocation& location, const ParseFunction& user, const NameHierarchy& usedNameHierarchy) = 0;
	virtual Id onEnumConstantUsageParsed(
		const ParseLocation& location, const ParseVariable& user, const NameHierarchy& usedNameHierarchy) = 0;
	virtual Id onTypeUsageParsed(const ParseTypeUsage& type, const ParseFunction& function) = 0;
	virtual Id onTypeUsageParsed(const ParseTypeUsage& type, const ParseVariable& variable) = 0;

	virtual Id onTemplateArgumentTypeOfTemplateRecordParsed(
		const ParseLocation& location, const NameHierarchy& argumentNameHierarchy,
		const NameHierarchy& templateNameHierarchy) = 0;
	virtual Id onTemplateArgumentTypeOfTemplateFunctionParsed(
		const ParseLocation& location, const NameHierarchy& argumentNameHierarchy,
		const ParseFunction& templateFunction) = 0;
	virtual Id onTemplateDefaultArgumentTypeParsed(
		const ParseTypeUsage& type,	const NameHierarchy& templateArgumentTypeNameHierarchy) = 0;
	virtual Id onTemplateRecordParameterTypeParsed(
		const ParseLocation& location, const NameHierarchy& templateParameterTypeNameHierarchy,
		const NameHierarchy& templateRecordNameHierarchy) = 0;
	virtual Id onTemplateRecordSpecializationParsed(
		const ParseLocation& location, const NameHierarchy& specializedRecordNameHierarchy,
		const RecordType specializedRecordType, const NameHierarchy& specializedFromNameHierarchy) = 0;
	virtual Id onTemplateMemberFunctionSpecializationParsed(
		const ParseLocation& location, const ParseFunction& instantiatedFunction, const ParseFunction& specializedFunction) = 0;

	virtual Id onTemplateFunctionParameterTypeParsed(
		const ParseLocation& location, const NameHierarchy& templateParameterTypeNameHierarchy,
		const ParseFunction function) = 0;
	virtual Id onTemplateFunctionSpecializationParsed(
		const ParseLocation& location, const ParseFunction specializedFunction, const ParseFunction templateFunction) = 0;

	virtual Id onFileParsed(const FileInfo& fileInfo) = 0;
	virtual Id onFileIncludeParsed(
		const ParseLocation& location, const FileInfo& fileInfo, const FileInfo& includedFileInfo) = 0;


	virtual Id onMacroDefineParsed(
		const ParseLocation& location, const NameHierarchy& macroNameHierarchy, const ParseLocation& scopeLocation) = 0;
	virtual Id onMacroExpandParsed(
		const ParseLocation& location, const NameHierarchy& macroNameHierarchy) = 0;

	virtual Id onCommentParsed(const ParseLocation& location) = 0;
};

#endif // PARSER_CLIENT_H
