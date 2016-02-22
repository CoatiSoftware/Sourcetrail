#ifndef STORAGE_H
#define STORAGE_H

#include <memory>
#include <vector>

#include "utility/file/FilePath.h"

#include "data/access/StorageAccess.h"
#include "data/HierarchyCache.h"
#include "data/graph/token_component/TokenComponentAccess.h"
#include "data/location/TokenLocationCollection.h"
#include "data/parser/ParserClient.h"
#include "data/search/SearchIndex.h"
#include "data/SqliteStorage.h"

class Storage
	: public ParserClient
	, public StorageAccess
{
public:
	Storage(const FilePath& dbPath);
	virtual ~Storage();

	Version getVersion() const;

	bool init();
	void clear();
	void clearCaches();

	std::set<FilePath> getDependingFilePaths(const std::set<FilePath>& filePaths);
	std::set<FilePath> getDependingFilePaths(const FilePath& filePath);

	void clearFileElements(const std::vector<FilePath>& filePaths);
	void removeUnusedNames();

	std::vector<FileInfo> getInfoOnAllFiles() const;
	const SearchIndex& getSearchIndex() const;

	void logStats() const;

	// ParserClient implementation
	virtual void startParsing();
	virtual void finishParsing();

	virtual void startParsingFile(const FilePath& filePath);
	virtual void finishParsingFile(const FilePath& filePath);

	virtual void onError(const ParseLocation& location, const std::string& message);
	virtual size_t getErrorCount() const;

	virtual Id onTypedefParsed(
		const ParseLocation& location, const NameHierarchy& typedefName, AccessType access);
	virtual Id onClassParsed(
		const ParseLocation& location, const NameHierarchy& nameHierarchy, AccessType access,
		const ParseLocation& scopeLocation);
	virtual Id onStructParsed(
		const ParseLocation& location, const NameHierarchy& nameHierarchy, AccessType access,
		const ParseLocation& scopeLocation);
	virtual Id onGlobalVariableParsed(const ParseLocation& location, const NameHierarchy& variable);
	virtual Id onFieldParsed(const ParseLocation& location, const NameHierarchy& field, AccessType access);
	virtual Id onFunctionParsed(
		const ParseLocation& location, const NameHierarchy& function, const ParseLocation& scopeLocation);
	virtual Id onMethodParsed(
		const ParseLocation& location, const NameHierarchy& method, AccessType access, AbstractionType abstraction,
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
		const ParseLocation& location, const NameHierarchy& overridden, const NameHierarchy& overrider);
	virtual Id onCallParsed(
		const ParseLocation& location, const NameHierarchy& caller, const NameHierarchy& callee);
	virtual Id onFieldUsageParsed(
		const ParseLocation& location, const NameHierarchy& userNameHierarchy, const NameHierarchy& usedNameHierarchy);
	virtual Id onGlobalVariableUsageParsed(
		const ParseLocation& location, const NameHierarchy& userNameHierarchy, const NameHierarchy& usedNameHierarchy);
	virtual Id onEnumConstantUsageParsed(
		const ParseLocation& location, const NameHierarchy& userNameHierarchy, const NameHierarchy& usedNameHierarchy);
	virtual Id onTypeUsageParsed(const ParseLocation& location, const NameHierarchy& user, const NameHierarchy& used);

	virtual Id onTemplateArgumentTypeParsed(
		const ParseLocation& location, const NameHierarchy& argumentTypeNameHierarchy,
		const NameHierarchy& templateNameHierarchy);
	virtual Id onTemplateDefaultArgumentTypeParsed(
		const ParseLocation& location, const NameHierarchy& defaultArgumentTypeNameHierarchy, 
		const NameHierarchy& templateParameterNameHierarchy);
	virtual Id onTemplateParameterTypeParsed(
		const ParseLocation& location, const NameHierarchy& templateParameterTypeNameHierarchy);
	virtual Id onTemplateSpecializationParsed(
		const ParseLocation& location, const NameHierarchy& specializedNameHierarchy,
		const NameHierarchy& specializedFromNameHierarchy);
	virtual Id onTemplateMemberFunctionSpecializationParsed(
		const ParseLocation& location, const NameHierarchy& instantiatedFunction, const NameHierarchy& specializedFunction);

	virtual Id onTemplateFunctionSpecializationParsed(
		const ParseLocation& location, const NameHierarchy specializedFunction, const NameHierarchy templateFunction);

	virtual Id onFileParsed(const FileInfo& fileInfo);
	virtual Id onFileIncludeParsed(
		const ParseLocation& location, const FileInfo& fileInfo, const FileInfo& includedFileInfo);

	virtual Id onMacroDefineParsed(
		const ParseLocation& location, const NameHierarchy& macroNameHierarchy, const ParseLocation& scopeLocation);
	virtual Id onMacroExpandParsed(const ParseLocation& location, const NameHierarchy& macroNameHierarchy);

	virtual Id onCommentParsed(const ParseLocation& location);

	// StorageAccess implementation
	virtual Id getIdForNodeWithNameHierarchy(const NameHierarchy& nameHierarchy) const;
	virtual Id getIdForNodeWithSearchNameHierarchy(const NameHierarchy& nameHierarchy) const;
	virtual Id getIdForEdge(
		Edge::EdgeType type, const NameHierarchy& fromNameHierarchy, const NameHierarchy& toNameHierarchy) const;

	virtual Id getIdForFirstNode() const;

	virtual NameHierarchy getNameHierarchyForNodeWithId(Id nodeId) const;
	virtual Node::NodeType getNodeTypeForNodeWithId(Id nodeId) const;

	virtual std::vector<SearchMatch> getAutocompletionMatches(const std::string& query) const;
	virtual std::vector<SearchMatch> getSearchMatchesForTokenIds(const std::vector<Id>& tokenIds) const;

	virtual std::shared_ptr<Graph> getGraphForAll() const;
	virtual std::shared_ptr<Graph> getGraphForActiveTokenIds(const std::vector<Id>& tokenIds, bool activeOnly) const;

	virtual std::vector<Id> getActiveTokenIdsForTokenIds(const std::vector<Id>& tokenIds) const;
	virtual std::vector<Id> getActiveTokenIdsForId(Id tokenId, Id* declarationId) const;

	virtual std::vector<Id> getNodeIdsForLocationIds(const std::vector<Id>& locationIds) const;

	virtual std::vector<Id> getTokenIdsForMatches(const std::vector<SearchMatch>& matches) const;
	virtual Id getTokenIdForFileNode(const FilePath& filePath) const;
	virtual std::vector<Id> getTokenIdsForAggregationEdge(Id sourceId, Id targetId) const;

	virtual std::shared_ptr<TokenLocationCollection> getTokenLocationsForTokenIds(const std::vector<Id>& tokenIds) const;
	virtual std::shared_ptr<TokenLocationCollection> getTokenLocationsForLocationIds(const std::vector<Id>& locationIds) const;
	virtual std::shared_ptr<TokenLocationFile> getTokenLocationsForFile(const std::string& filePath) const;
	virtual std::shared_ptr<TokenLocationFile> getTokenLocationsForLinesInFile(
		const std::string& filePath, uint firstLineNumber, uint lastLineNumber
	) const;

	virtual TokenLocationCollection getErrorTokenLocations(std::vector<std::string>* errorMessages) const;
	virtual std::shared_ptr<TokenLocationFile> getCommentLocationsInFile(const FilePath& filePath) const;

	virtual std::shared_ptr<TextAccess> getFileContent(const FilePath& filePath) const;
	virtual TimePoint getFileModificationTime(const FilePath& filePath) const;

	virtual StorageStats getStorageStats() const;

private:
	Id addNodeHierarchy(Node::NodeType nodeType, NameHierarchy nameHierarchy, bool defined);
	Id addSourceLocation(Id elementNodeId, const ParseLocation &location, bool isScope = false);

	Id addEdge(Id sourceNodeId, Id targetNodeId, Edge::EdgeType type);
	Id addEdge(Id sourceNodeId, Id targetNodeId, Edge::EdgeType type, ParseLocation location);

	Id getFileNodeId(const FilePath& filePath) const;
	FilePath getFileNodePath(Id fileId) const;

	Id getLastVisibleParentNodeId(const Id nodeId) const;
	std::vector<Id> getAllChildNodeIds(const Id nodeId) const;

	void addEdgeAndAllChildrenToGraph(const Id edgeId, Graph* graph) const;
	Node* addNodeAndAllChildrenToGraph(const Id nodeId, Graph* graph) const;
	void addAggregationEdgesToGraph(const Id nodeId, Graph* graph) const;

	void addNodesToGraph(const std::vector<Id> nodeIds, Graph* graph) const;
	void addEdgesToGraph(const std::vector<Id> edgeIds, Graph* graph) const;

	TokenComponentAccess::AccessType convertAccessType(ParserClient::AccessType access) const;
	void addAccess(Id nodeId, TokenComponentAccess::AccessType access);
	void addAccess(Id nodeId, ParserClient::AccessType access);
	TokenComponentAccess::AccessType getAccess(Id nodeId) const;

	void addComponentAccessToGraph(Graph* graph) const;

	void buildSearchIndex();
	void buildHierarchyCache();

	void log(std::string type, std::string str, const ParseLocation& location) const;

	SearchIndex m_tokenIndex;
	SearchIndex m_commandIndex;

	SqliteStorage m_sqliteStorage;

	mutable std::map <FilePath, Id> m_fileNodeIds;
	HierarchyCache m_hierarchyCache;

	mutable SearchResults m_cachedResults;
	mutable std::string m_cachedQuery;
};

#endif // STORAGE_H
