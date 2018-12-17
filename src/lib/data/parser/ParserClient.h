#ifndef PARSER_CLIENT_H
#define PARSER_CLIENT_H

#include <string>

#include "AccessKind.h"
#include "DefinitionKind.h"
#include "NameHierarchy.h"
#include "ParseLocation.h"
#include "ReferenceKind.h"
#include "SymbolKind.h"
#include "types.h"

class ParserClient
{
public:
	virtual ~ParserClient() = default;

	virtual Id recordFile(const FilePath& filePath, bool indexed) = 0;
	virtual void recordFileLanguage(Id fileId, const std::wstring& languageIdentifier) = 0;

	virtual Id recordSymbol(const NameHierarchy& symbolName) = 0;
	virtual void recordSymbolKind(Id symbolId, SymbolKind symbolKind) = 0;
	virtual void recordAccessKind(Id symbolId, AccessKind accessKind) = 0;
	virtual void recordDefinitionKind(Id symbolId, DefinitionKind definitionKind) = 0;

	virtual Id recordReference(ReferenceKind referenceKind, Id referencedSymbolId, Id contextSymbolId,
		const ParseLocation& location) = 0;

	virtual void recordLocalSymbol(const std::wstring& name, const ParseLocation& location) = 0;
	virtual void recordLocation(Id elementId, const ParseLocation& location, ParseLocationType type) = 0;
	virtual void recordComment(const ParseLocation& location) = 0;

	virtual void recordError(const std::wstring& message, bool fatal, bool indexed, const FilePath& translationUnit, const ParseLocation& location) = 0;

	virtual bool hasContent() const = 0;
};

#endif // PARSER_CLIENT_H
