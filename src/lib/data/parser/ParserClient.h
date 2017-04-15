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

	virtual void onError(const ParseLocation& location, const std::string& message, bool fatal, bool indexed) = 0;
	virtual void onLocalSymbolParsed(const std::string& name, const ParseLocation& location) = 0;
	virtual void onFileParsed(const FileInfo& fileInfo) = 0;
	virtual void onCommentParsed(const ParseLocation& location) = 0;

	void onErrorParsed(const ParseLocation& location, const std::string& message, bool fatal, bool indexed);

	bool hasFatalErrors() const;

	bool cancelOnFatalErrors() const;
	void setCancelOnFatalErrors(bool cancelOnFatalErrors);

protected:
	bool m_hasFatalErrors;
	bool m_cancelOnFatalErrors;
};

#endif // PARSER_CLIENT_H
