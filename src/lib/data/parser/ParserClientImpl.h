#ifndef PARSER_CLIENT_IMPL_H
#define PARSER_CLIENT_IMPL_H

#include <set>

#include "DefinitionKind.h"
#include "Node.h"
#include "IntermediateStorage.h"
#include "LocationType.h"
#include "ParserClient.h"

class ParserClientImpl
	: public ParserClient
{
public:
	ParserClientImpl(IntermediateStorage* const storage);

	Id recordSymbol(
		const NameHierarchy& symbolName, SymbolKind symbolKind,
		AccessKind access, DefinitionKind definitionKind) override;

	Id recordSymbolWithLocation(
		const NameHierarchy& symbolName, SymbolKind symbolKind,
		const ParseLocation& location,
		AccessKind access, DefinitionKind definitionKind) override;

	Id recordSymbolWithLocationAndScope(
		const NameHierarchy& symbolName, SymbolKind symbolKind,
		const ParseLocation& location, const ParseLocation& scopeLocation,
		AccessKind access, DefinitionKind definitionKind) override;

	Id recordSymbolWithLocationAndScopeAndSignature(
		const NameHierarchy& symbolName, SymbolKind symbolKind,
		const ParseLocation& location, const ParseLocation& scopeLocation, const ParseLocation& signatureLocation,
		AccessKind access, DefinitionKind definitionKind) override;

	void recordReference(
		ReferenceKind referenceKind, const NameHierarchy& referencedName, const NameHierarchy& contextName,
		const ParseLocation& location) override;

	void recordQualifierLocation(
		const NameHierarchy& qualifierName, const ParseLocation& location) override;

	void recordLocalSymbol(const std::wstring& name, const ParseLocation& location) override;
	void recordFile(const FilePath& filePath, bool indexed) override;
	void recordComment(const ParseLocation& location) override;

private:
	void doRecordError(
		const ParseLocation& location, const std::wstring& message, bool fatal, bool indexed, const FilePath& sourceFilePath) override;

	NodeType symbolKindToNodeType(SymbolKind symbolType) const;
	Edge::EdgeType referenceKindToEdgeType(ReferenceKind referenceKind) const;
	void addAccess(Id nodeId, AccessKind access);

	Id addNodeHierarchy(const NameHierarchy& nameHierarchy, NodeType nodeType = NodeType::NODE_SYMBOL);
	Id addFileName(const FilePath& filePath);
	Id addEdge(int type, Id sourceId, Id targetId);

	void addSourceLocation(Id elementId, const ParseLocation& location, LocationType type);
	void addError(const std::wstring& message, bool fatal, bool indexed,
		const ParseLocation& location, const FilePath& sourceFilePath);

	IntermediateStorage* const m_storage;
	std::map<std::wstring, Id> m_fileIdMap;
};

#endif // PARSER_CLIENT_IMPL_H
