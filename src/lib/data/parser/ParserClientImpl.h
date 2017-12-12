#ifndef PARSER_CLIENT_IMPL_H
#define PARSER_CLIENT_IMPL_H

#include <set>

#include "data/DefinitionKind.h"
#include "data/graph/Node.h"
#include "data/storage/IntermediateStorage.h"
#include "data/parser/ParserClient.h"

class ParserClientImpl
	: public ParserClient
{
public:
	ParserClientImpl();
	virtual ~ParserClientImpl();

	void setStorage(std::shared_ptr<IntermediateStorage> storage);
	void resetStorage();

	virtual Id recordSymbol(
		const NameHierarchy& symbolName, SymbolKind symbolKind,
		AccessKind access, DefinitionKind definitionKind) override;

	virtual Id recordSymbol(
		const NameHierarchy& symbolName, SymbolKind symbolKind,
		const ParseLocation& location,
		AccessKind access, DefinitionKind definitionKind) override;

	virtual Id recordSymbol(
		const NameHierarchy& symbolName, SymbolKind symbolKind,
		const ParseLocation& location, const ParseLocation& scopeLocation,
		AccessKind access, DefinitionKind definitionKind) override;

	virtual void recordReference(
		ReferenceKind referenceKind, const NameHierarchy& referencedName, const NameHierarchy& contextName,
		const ParseLocation& location) override;

	virtual void recordQualifierLocation(
		const NameHierarchy& qualifierName, const ParseLocation& location) override;
	
	virtual void recordLocalSymbol(const std::string& name, const ParseLocation& location) override;
	virtual void recordFile(const FileInfo& fileInfo) override;
	virtual void recordComment(const ParseLocation& location) override;

private:
	virtual void doRecordError(
		const ParseLocation& location, const std::string& message, bool fatal, bool indexed) override;

	NodeType symbolKindToNodeType(SymbolKind symbolType) const;
	Edge::EdgeType referenceKindToEdgeType(ReferenceKind referenceKind) const;
	void addAccess(Id nodeId, AccessKind access);
	Id addNodeHierarchy(NameHierarchy nameHierarchy, NodeType nodeType = NodeType::NODE_SYMBOL);

	Id addNode(NodeType nodeType, NameHierarchy nameHierarchy);
	void addFile(Id id, const FilePath& filePath, const std::string& modificationTime);
	void addSymbol(Id id, DefinitionKind definitionKind);
	Id addEdge(int type, Id sourceId, Id targetId);
	Id addLocalSymbol(const std::string& name);
	void addSourceLocation(Id elementId, const ParseLocation& location, int type);
	void addComponentAccess(Id nodeId , int type);
	void addCommentLocation(const ParseLocation& location);
	void addError(const std::string& message, bool fatal, bool indexed,
		const ParseLocation& location);

	std::shared_ptr<IntermediateStorage> m_storage;
};

#endif // PARSER_CLIENT_IMPL_H
