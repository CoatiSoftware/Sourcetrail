#ifndef PARSER_CLIENT_H
#define PARSER_CLIENT_H

#include <string>
#include <vector>

#include "data/name/NameHierarchy.h"
#include "data/parser/AccessKind.h"
#include "data/parser/ReferenceKind.h"
#include "data/parser/SymbolKind.h"
#include "data/DefinitionKind.h"
#include "utility/file/FileInfo.h"
#include "utility/types.h"

struct ParseLocation;
class DataType;

class ParserClient
{
public:
	static std::wstring addAccessPrefix(const std::wstring& str, AccessKind access);
	static std::wstring addStaticPrefix(const std::wstring& str, bool isStatic);
	static std::wstring addConstPrefix(const std::wstring& str, bool isConst, bool atFront);
	static std::wstring addLocationSuffix(const std::wstring& str, const ParseLocation& location);
	static std::wstring addLocationSuffix(
		const std::wstring& str, const ParseLocation& location, const ParseLocation& scopeLocation);

	ParserClient();
	virtual ~ParserClient();

	virtual Id recordSymbol(
		const NameHierarchy& symbolName, SymbolKind symbolKind,
		AccessKind access, DefinitionKind definitionKind) = 0;

	virtual Id recordSymbol(
		const NameHierarchy& symbolName, SymbolKind symbolKind,
		const ParseLocation& location,
		AccessKind access, DefinitionKind definitionKind) = 0;

	virtual Id recordSymbol(
		const NameHierarchy& symbolName, SymbolKind symbolKind,
		const ParseLocation& location, const ParseLocation& scopeLocation,
		AccessKind access, DefinitionKind definitionKind) = 0;

	virtual void recordReference(
		ReferenceKind referenceKind, const NameHierarchy& referencedName, const NameHierarchy& contextName,
		const ParseLocation& location) = 0;

	virtual void recordQualifierLocation(
		const NameHierarchy& qualifierName, const ParseLocation& location) = 0;

	void recordError(
		const ParseLocation& location, const std::wstring& message, bool fatal, bool indexed);

	virtual void recordLocalSymbol(const std::wstring& name, const ParseLocation& location) = 0;
	virtual void recordFile(const FileInfo& fileInfo, bool indexed) = 0;
	virtual void recordComment(const ParseLocation& location) = 0;

	bool hasFatalErrors() const;

protected:
	virtual void doRecordError(
		const ParseLocation& location, const std::wstring& message, bool fatal, bool indexed) = 0;

	bool m_hasFatalErrors;
};

#endif // PARSER_CLIENT_H
