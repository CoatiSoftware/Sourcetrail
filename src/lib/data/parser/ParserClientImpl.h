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
		AccessKind access, DefinitionKind definitionKind);

	virtual Id recordSymbol(
		const NameHierarchy& symbolName, SymbolKind symbolKind,
		const ParseLocation& location,
		AccessKind access, DefinitionKind definitionKind);

	virtual Id recordSymbol(
		const NameHierarchy& symbolName, SymbolKind symbolKind,
		const ParseLocation& location, const ParseLocation& scopeLocation,
		AccessKind access, DefinitionKind definitionKind);

	virtual void recordReference(
		ReferenceKind referenceKind, const NameHierarchy& referencedName, const NameHierarchy& contextName,
		const ParseLocation& location);

	virtual void recordQualifierLocation(
		const NameHierarchy& qualifierName, const ParseLocation& location);

	virtual void onError(const ParseLocation& location, const std::string& message, const std::string& commandline,
		bool fatal, bool indexed);
	
	virtual void onLocalSymbolParsed(const std::string& name, const ParseLocation& location);
	virtual void onFileParsed(const FileInfo& fileInfo);
	virtual void onCommentParsed(const ParseLocation& location);

private:
	Node::NodeType symbolKindToNodeType(SymbolKind symbolType) const;
	Edge::EdgeType referenceKindToEdgeType(ReferenceKind referenceKind) const;
	void addAccess(Id nodeId, AccessKind access);
	Id addNodeHierarchy(NameHierarchy nameHierarchy, Node::NodeType nodeType = Node::NODE_NON_INDEXED);

	Id addNode(Node::NodeType nodeType, NameHierarchy nameHierarchy);
	void addFile(Id id, const FilePath& filePath, const std::string& modificationTime);
	void addSymbol(Id id, DefinitionKind definitionKind);
	Id addEdge(int type, Id sourceId, Id targetId);
	Id addLocalSymbol(const std::string& name);
	void addSourceLocation(Id elementId, const ParseLocation& location, int type);
	void addComponentAccess(Id nodeId , int type);
	void addCommentLocation(const ParseLocation& location);
	void addError(const std::string& message, const std::string& commandline, bool fatal, bool indexed,
		const ParseLocation& location);

	std::shared_ptr<IntermediateStorage> m_storage;
};

#endif // PARSER_CLIENT_IMPL_H
