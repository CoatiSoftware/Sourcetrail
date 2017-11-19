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
	static std::string addAccessPrefix(const std::string& str, AccessKind access);
	static std::string addStaticPrefix(const std::string& str, bool isStatic);
	static std::string addConstPrefix(const std::string& str, bool isConst, bool atFront);
	static std::string addLocationSuffix(const std::string& str, const ParseLocation& location);
	static std::string addLocationSuffix(
		const std::string& str, const ParseLocation& location, const ParseLocation& scopeLocation);

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

	void recordError(const ParseLocation& location, const std::string& message, const std::string& commandline,
		bool fatal, bool indexed);
	
	virtual void recordLocalSymbol(const std::string& name, const ParseLocation& location) = 0;
	virtual void recordFile(const FileInfo& fileInfo) = 0;
	virtual void recordComment(const ParseLocation& location) = 0;

	bool hasFatalErrors() const;

protected:
	virtual void doRecordError(const ParseLocation& location, const std::string& message, const std::string& commandline,
		bool fatal, bool indexed) = 0;

	bool m_hasFatalErrors;
};

#endif // PARSER_CLIENT_H
