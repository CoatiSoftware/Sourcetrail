#ifndef PARSER_CLIENT_H
#define PARSER_CLIENT_H

#include <string>
#include <vector>

#include "data/name/NameHierarchy.h"
#include "data/parser/AccessKind.h"
#include "data/parser/ReferenceKind.h"
#include "data/parser/SymbolKind.h"
#include "utility/file/FileInfo.h"
#include "utility/types.h"

struct ParseLocation;
class DataType;

class ParserClient
{
public:
	enum AbstractionType {
		ABSTRACTION_VIRTUAL,
		ABSTRACTION_PURE_VIRTUAL,
		ABSTRACTION_NONE
	};

	static std::string addAccessPrefix(const std::string& str, AccessKind access);
	static std::string addAbstractionPrefix(const std::string& str, AbstractionType abstraction);
	static std::string addStaticPrefix(const std::string& str, bool isStatic);
	static std::string addConstPrefix(const std::string& str, bool isConst, bool atFront);
	static std::string addLocationSuffix(const std::string& str, const ParseLocation& location);
	static std::string addLocationSuffix(
		const std::string& str, const ParseLocation& location, const ParseLocation& scopeLocation);

	ParserClient();
	virtual ~ParserClient();

	virtual void startParsingFile() = 0;
	virtual void finishParsingFile() = 0;

	virtual Id recordSymbol(
		const NameHierarchy& symbolName, SymbolKind symbolType,
		AccessKind access = ACCESS_NONE, bool isImplicit = false) = 0;

	virtual Id recordSymbol(
		const NameHierarchy& symbolName, SymbolKind symbolType,
		const ParseLocation& location,
		AccessKind access = ACCESS_NONE, bool isImplicit = false) = 0;

	virtual Id recordSymbol(
		const NameHierarchy& symbolName, SymbolKind symbolType,
		const ParseLocation& location, const ParseLocation& scopeLocation,
		AccessKind access = ACCESS_NONE, bool isImplicit = false) = 0;

	virtual void recordReference(
		ReferenceKind referenceKind, const NameHierarchy& referencedName, const NameHierarchy& contextName,
		const ParseLocation& location) = 0;

	virtual void onError(const ParseLocation& location, const std::string& message, bool fatal, bool indexed) = 0;

	virtual void onTypedefParsed(
		const ParseLocation& location, const NameHierarchy& typedefName, AccessKind access, bool isImplicit) = 0;
	virtual void onClassParsed(
		const ParseLocation& location, const NameHierarchy& nameHierarchy, AccessKind access,
		const ParseLocation& scopeLocation, bool isImplicit) = 0;
	virtual void onStructParsed(
		const ParseLocation& location, const NameHierarchy& nameHierarchy, AccessKind access,
		const ParseLocation& scopeLocation, bool isImplicit) = 0;
	virtual void onGlobalVariableParsed(const ParseLocation& location, const NameHierarchy& variable, bool isImplicit) = 0;
	virtual void onFieldParsed(const ParseLocation& location, const NameHierarchy& field, AccessKind access, bool isImplicit) = 0;
	virtual void onFunctionParsed(
		const ParseLocation& location, const NameHierarchy& function, const ParseLocation& scopeLocation, bool isImplicit) = 0;
	virtual void onMethodParsed(
		const ParseLocation& location, const NameHierarchy& method, AccessKind access, AbstractionType abstraction,
		const ParseLocation& scopeLocation, bool isImplicit) = 0;
	virtual void onNamespaceParsed(
		const ParseLocation& location, const NameHierarchy& nameHierarchy,
		const ParseLocation& scopeLocation, bool isImplicit) = 0;
	virtual void onEnumParsed(
		const ParseLocation& location, const NameHierarchy& nameHierarchy, AccessKind access,
		const ParseLocation& scopeLocation, bool isImplicit) = 0;
	virtual void onEnumConstantParsed(const ParseLocation& location, const NameHierarchy& nameHierarchy, bool isImplicit) = 0;
	virtual void onTemplateParameterTypeParsed(
		const ParseLocation& location, const NameHierarchy& templateParameterTypeNameHierarchy, bool isImplicit) = 0;
	virtual void onLocalSymbolParsed(const std::string& name, const ParseLocation& location) = 0;
	virtual void onFileParsed(const FileInfo& fileInfo) = 0;
	virtual void onMacroDefineParsed(
		const ParseLocation& location, const NameHierarchy& macroNameHierarchy, const ParseLocation& scopeLocation) = 0;
	virtual void onCommentParsed(const ParseLocation& location) = 0;

	virtual void onInheritanceParsed(
		const ParseLocation& location, const NameHierarchy& nameHierarchy,
		const NameHierarchy& baseNameHierarchy) = 0;
	virtual void onMethodOverrideParsed(
		const ParseLocation& location, const NameHierarchy& overridden, const NameHierarchy& overrider) = 0;
	virtual void onCallParsed(
		const ParseLocation& location, const NameHierarchy& caller, const NameHierarchy& callee) = 0;
	virtual void onUsageParsed(
		const ParseLocation& location, const NameHierarchy& userName, SymbolKind usedType, const NameHierarchy& usedName) = 0;
	virtual void onTypeUsageParsed(const ParseLocation& location, const NameHierarchy& user, const NameHierarchy& used) = 0;

	virtual void onTemplateArgumentTypeParsed(
		const ParseLocation& location, const NameHierarchy& argumentTypeNameHierarchy,
		const NameHierarchy& templateNameHierarchy) = 0;
	virtual void onTemplateDefaultArgumentTypeParsed(
		const ParseLocation& location, const NameHierarchy& defaultArgumentTypeNameHierarchy,
		const NameHierarchy& templateParameterNameHierarchy) = 0;
	virtual void onTemplateSpecializationParsed(
		const ParseLocation& location, const NameHierarchy& specializedNameHierarchy,
		const NameHierarchy& specializedFromNameHierarchy) = 0;
	virtual void onTemplateMemberFunctionSpecializationParsed(
		const ParseLocation& location, const NameHierarchy& instantiatedFunction, const NameHierarchy& specializedFunction) = 0;

	virtual void onFileIncludeParsed(
		const ParseLocation& location, const FileInfo& fileInfo, const FileInfo& includedFileInfo) = 0;

	virtual void onMacroExpandParsed(
		const ParseLocation& location, const NameHierarchy& macroNameHierarchy) = 0;
};

#endif // PARSER_CLIENT_H
