#ifndef PARSER_CLIENT_IMPL_H
#define PARSER_CLIENT_IMPL_H

#include <set>

#include "DefinitionKind.h"
#include "IntermediateStorage.h"
#include "LocationType.h"
#include "Node.h"
#include "ParserClient.h"

class ParserClientImpl: public ParserClient
{
public:
	ParserClientImpl(IntermediateStorage* const storage);

	Id recordFile(const FilePath& filePath, bool indexed) override;
	void recordFileLanguage(Id fileId, const std::wstring& languageIdentifier) override;

	Id recordSymbol(const NameHierarchy& symbolName) override;
	void recordSymbolKind(Id symbolId, SymbolKind symbolKind) override;
	void recordAccessKind(Id symbolId, AccessKind accessKind) override;
	void recordDefinitionKind(Id symbolId, DefinitionKind definitionKind) override;

	Id recordReference(
		ReferenceKind referenceKind,
		Id referencedSymbolId,
		Id contextSymbolId,
		const ParseLocation& location) override;

	void recordLocalSymbol(const std::wstring& name, const ParseLocation& location) override;
	void recordLocation(Id elementId, const ParseLocation& location, ParseLocationType type) override;
	void recordComment(const ParseLocation& location) override;

	void recordError(
		const std::wstring& message,
		bool fatal,
		bool indexed,
		const FilePath& translationUnit,
		const ParseLocation& location) override;

	bool hasContent() const override;

private:
	NodeKind symbolKindToNodeKind(SymbolKind symbolType) const;
	Edge::EdgeType referenceKindToEdgeType(ReferenceKind referenceKind) const;
	LocationType parseLocationTypeToLocationType(ParseLocationType type) const;

	void addAccess(Id nodeId, AccessKind access);

	Id addNodeHierarchy(const NameHierarchy& nameHierarchy);
	Id addFileName(const FilePath& filePath);
	Id addEdge(int type, Id sourceId, Id targetId);

	void addSourceLocation(Id elementId, const ParseLocation& location, LocationType type);

	IntermediateStorage* const m_storage;
	std::map<std::wstring, Id> m_fileIdMap;
};

#endif	  // PARSER_CLIENT_IMPL_H
