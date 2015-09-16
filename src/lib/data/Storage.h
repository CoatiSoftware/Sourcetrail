#ifndef STORAGE_H
#define STORAGE_H

#include <memory>
#include <vector>

#include "utility/file/FilePath.h"

#include "data/access/StorageAccess.h"
//#include "data/graph/token_component/TokenComponentAbstraction.h"
//#include "data/graph/token_component/TokenComponentAccess.h"
#include "data/location/TokenLocationCollection.h"
#include "data/parser/ParserClient.h"
#include "data/search/SearchIndex.h"
#include "data/SqliteStorage.h"

class Storage
	: public ParserClient
	, public StorageAccess
{
public:
	Storage();
	virtual ~Storage();

	void clear();
	void clearFileElements(const std::set<FilePath>& filePaths);
	void clearFileElements(const FilePath& filePath);
	std::set<FilePath> getDependingFilePaths(const std::set<FilePath>& filePaths);
	std::set<FilePath> getDependingFilePaths(const FilePath& filePath);
	void removeUnusedNames();

	void logGraph() const;
	void logLocations() const;
	void logIndex() const;
	void logStats() const;

	// ParserClient implementation
	virtual void prepareParsingFile();
	virtual void finishParsingFile();

	virtual void onError(const ParseLocation& location, const std::string& message);
	virtual size_t getErrorCount() const;

	virtual Id onTypedefParsed(
		const ParseLocation& location, const NameHierarchy& nameHierarchy,
		const ParseTypeUsage& underlyingType, AccessType access);
	virtual Id onClassParsed(
		const ParseLocation& location, const NameHierarchy& nameHierarchy, AccessType access,
		const ParseLocation& scopeLocation);
	virtual Id onStructParsed(
		const ParseLocation& location, const NameHierarchy& nameHierarchy, AccessType access,
		const ParseLocation& scopeLocation);

	virtual Id onGlobalVariableParsed(const ParseLocation& location, const ParseVariable& variable);
	virtual Id onFieldParsed(const ParseLocation& location, const ParseVariable& variable, AccessType access);

	virtual Id onFunctionParsed(
		const ParseLocation& location, const ParseFunction& function, const ParseLocation& scopeLocation);
	virtual Id onMethodParsed(
		const ParseLocation& location, const ParseFunction& method, AccessType access, AbstractionType abstraction,
		const ParseLocation& scopeLocation);

	virtual Id onNamespaceParsed(
		const ParseLocation& location, const NameHierarchy& nameHierarchy, const ParseLocation& scopeLocation);

	virtual Id onEnumParsed(
		const ParseLocation& location, const NameHierarchy& nameHierarchy, AccessType access,
		const ParseLocation& scopeLocation);
	virtual Id onEnumConstantParsed(const ParseLocation& location, const NameHierarchy& nameHierarchy);

	virtual Id onInheritanceParsed(
		const ParseLocation& location, const NameHierarchy& childNameHierarchy,
		const NameHierarchy& parentNameHierarchy, AccessType access);
	virtual Id onMethodOverrideParsed(
		const ParseLocation& location, const ParseFunction& base, const ParseFunction& overrider);
	virtual Id onCallParsed(
		const ParseLocation& location, const ParseFunction& caller, const ParseFunction& callee);
	virtual Id onCallParsed(
		const ParseLocation& location, const ParseVariable& caller, const ParseFunction& callee);
	virtual Id onFieldUsageParsed(
		const ParseLocation& location, const ParseFunction& user, const NameHierarchy& usedNameHierarchy);
	virtual Id onFieldUsageParsed(
		const ParseLocation& location, const ParseVariable& user, const NameHierarchy& usedNameHierarchy);
	virtual Id onGlobalVariableUsageParsed(
		const ParseLocation& location, const ParseFunction& user, const NameHierarchy& usedNameHierarchy);
	virtual Id onGlobalVariableUsageParsed(
		const ParseLocation& location, const ParseVariable& user, const NameHierarchy& usedNameHierarchy);
	virtual Id onEnumConstantUsageParsed(
		const ParseLocation& location, const ParseFunction& user, const NameHierarchy& usedNameHierarchy);
	virtual Id onEnumConstantUsageParsed(
		const ParseLocation& location, const ParseVariable& user, const NameHierarchy& usedNameHierarchy);
	virtual Id onTypeUsageParsed(const ParseTypeUsage& typeUsage, const ParseFunction& function);
	virtual Id onTypeUsageParsed(const ParseTypeUsage& typeUsage, const ParseVariable& variable);

	virtual Id onTemplateArgumentTypeParsed(
		const ParseLocation& location, const NameHierarchy& argumentNameHierarchy,
		const NameHierarchy& templateNameHierarchy);
	virtual Id onTemplateDefaultArgumentTypeParsed(
		const ParseTypeUsage& defaultArgumentTypeUsage,	const NameHierarchy& templateArgumentTypeNameHierarchy);
	virtual Id onTemplateRecordParameterTypeParsed(
		const ParseLocation& location, const NameHierarchy& templateParameterTypeNameHierarchy,
		const NameHierarchy& templateRecordNameHierarchy);
	virtual Id onTemplateRecordSpecializationParsed(
		const ParseLocation& location, const NameHierarchy& specializedRecordNameHierarchy,
		const RecordType specializedRecordType, const NameHierarchy& specializedFromNameHierarchy);
	virtual Id onTemplateFunctionParameterTypeParsed(
		const ParseLocation& location, const NameHierarchy& templateParameterTypeNameHierarchy, const ParseFunction function);
	virtual Id onTemplateFunctionSpecializationParsed(
		const ParseLocation& location, const ParseFunction specializedFunction, const ParseFunction templateFunction);

	virtual Id onFileParsed(const FileInfo& fileInfo);
	virtual Id onFileIncludeParsed(
		const ParseLocation& location, const FileInfo& fileInfo, const FileInfo& includedFileInfo);

	// StorageAccess implementation
	virtual Id getIdForNodeWithName(const std::string& fullName) const;
	virtual Id getIdForEdgeWithName(const std::string& name) const;

	virtual std::vector<FileInfo> getInfoOnAllFiles() const;

	virtual std::string getNameForNodeWithId(Id nodeId) const;
	virtual Node::NodeType getNodeTypeForNodeWithId(Id nodeId) const;
	virtual std::vector<SearchMatch> getAutocompletionMatches(
		const std::string& query, const std::string& word) const;

	virtual std::shared_ptr<Graph> getGraphForActiveTokenIds(const std::vector<Id>& tokenIds) const;

	virtual std::vector<Id> getActiveTokenIdsForId(Id tokenId, Id* declarationId) const;
	virtual Id getActiveNodeIdForLocationId(Id locationId) const;

	virtual std::vector<Id> getTokenIdsForMatches(const std::vector<SearchMatch>& matches) const;
	virtual Id getTokenIdForFileNode(const FilePath& filePath) const;
	virtual std::vector<Id> getTokenIdsForAggregationEdge(Id sourceId, Id targetId) const;

	virtual TokenLocationCollection getTokenLocationsForTokenIds(const std::vector<Id>& tokenIds) const;
	virtual TokenLocationCollection getTokenLocationsForLocationIds(const std::vector<Id>& locationIds) const;
	virtual std::shared_ptr<TokenLocationFile> getTokenLocationsForFile(const std::string& filePath) const;
	virtual std::shared_ptr<TokenLocationFile> getTokenLocationsForLinesInFile(
		const std::string& filePath, uint firstLineNumber, uint lastLineNumber
	) const;

	virtual TokenLocationCollection getErrorTokenLocations(std::vector<std::string>* errorMessages) const;

	virtual std::shared_ptr<TokenLocationFile> getTokenLocationOfParentScope(const TokenLocation* child) const;

	const SearchIndex& getSearchIndex() const;

private:
	Id addNodeHierarchy(Node::NodeType nodeType, NameHierarchy nameHierarchy);
	Id addNodeHierarchyWithDistinctSignature(Node::NodeType type, const ParseFunction& function);
	Id addNameHierarchyElements(NameHierarchy nameHierarchy);
	int addSourceLocation(int elementNodeId, const ParseLocation& location, bool isScope = false);
	Id addEdge(Id sourceNodeId, Id targetNodeId, Edge::EdgeType type, ParseLocation location);

	Id getLastParentNodeId(const Id nodeId) const;
	std::vector<Id> getDirectChildNodeIds(const Id nodeId) const;
	std::vector<Id> getAllChildNodeIds(const Id nodeId) const;

	void addEdgeAndAllChildrenToGraph(const Id edgeId, Graph* graph) const;
	Node* addNodeAndAllChildrenToGraph(const Id nodeId, Graph* graph) const;
	void addAggregationEdgesToGraph(const Id nodeId, Graph* graph) const;
	Node* addNodeToGraph(const Id nodeId, Graph* graph) const;

	SearchIndex m_tokenIndex;
	SqliteStorage m_sqliteStorage;

	TokenLocationCollection m_errorLocationCollection;
	std::vector<std::string> m_errorMessages;
};

#endif // STORAGE_H
