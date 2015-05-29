#ifndef STORAGE_H
#define STORAGE_H

#include <memory>
#include <vector>

#include "utility/file/FilePath.h"

#include "data/access/StorageAccess.h"
#include "data/graph/StorageGraph.h"
#include "data/graph/token_component/TokenComponentAbstraction.h"
#include "data/graph/token_component/TokenComponentAccess.h"
#include "data/location/TokenLocationCollection.h"
#include "data/parser/ParserClient.h"
#include "data/search/SearchIndex.h"

class Storage
	: public ParserClient
	, public StorageAccess
{
public:
	Storage();
	virtual ~Storage();

	void clear();
	void clearFileData(const std::set<FilePath>& filePaths);
	std::set<FilePath> getDependingFilePathsAndRemoveFileNodes(const std::set<FilePath>& filePaths);

	void logGraph() const;
	void logLocations() const;
	void logIndex() const;
	void logStats() const;

	// ParserClient implementation
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
		const ParseLocation& location, const NameHierarchy& nameHierarchy,
		const NameHierarchy& baseNameHierarchy, AccessType access);
	virtual Id onMethodOverrideParsed(
		const ParseLocation& location, const ParseFunction& base, const ParseFunction& overrider);
	virtual Id onCallParsed(
		const ParseLocation& location, const ParseFunction& caller, const ParseFunction& callee);
	virtual Id onCallParsed(
		const ParseLocation& location, const ParseVariable& caller, const ParseFunction& callee);
	Id onVariableUsageParsed(
		const std::string kind, const ParseLocation& location, const ParseFunction& user,
		const NameHierarchy& usedNameHierarchy); // helper
	virtual Id onFieldUsageParsed(
		const ParseLocation& location, const ParseFunction& user, const NameHierarchy& usedNameHierarchy);
	virtual Id onGlobalVariableUsageParsed(
		const ParseLocation& location, const ParseFunction& user, const NameHierarchy& usedNameHierarchy);
	virtual Id onGlobalVariableUsageParsed(
		const ParseLocation& location, const ParseVariable& user, const NameHierarchy& usedNameHierarchy);
	virtual Id onEnumConstantUsageParsed(
		const ParseLocation& location, const ParseFunction& user, const NameHierarchy& usedNameHierarchy);
	virtual Id onEnumConstantUsageParsed(
		const ParseLocation& location, const ParseVariable& user, const NameHierarchy& usedNameHierarchy);
	virtual Id onTypeUsageParsed(const ParseTypeUsage& type, const ParseFunction& function);
	virtual Id onTypeUsageParsed(const ParseTypeUsage& type, const ParseVariable& variable);

	virtual Id onTemplateArgumentTypeParsed(
		const ParseLocation& location, const NameHierarchy& argumentNameHierarchy,
		const NameHierarchy& templateNameHierarchy);
	virtual Id onTemplateDefaultArgumentTypeParsed(
		const ParseTypeUsage& type,	const NameHierarchy& templateArgumentTypeNameHierarchy);
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

	virtual Id onFileParsed(const std::string& filePath);
	virtual Id onFileIncludeParsed(
		const ParseLocation& location, const std::string& filePath, const std::string& includedPath);

	// StorageAccess implementation
	virtual Id getIdForNodeWithName(const std::string& fullName) const;
	virtual std::string getNameForNodeWithId(Id id) const;
	virtual Node::NodeType getNodeTypeForNodeWithId(Id id) const;
	virtual std::vector<SearchMatch> getAutocompletionMatches(
		const std::string& query, const std::string& word) const;

	virtual std::shared_ptr<Graph> getGraphForActiveTokenIds(const std::vector<Id>& tokenIds) const;

	virtual std::vector<Id> getActiveTokenIdsForId(Id tokenId, Id* declarationId) const;
	virtual std::vector<Id> getActiveTokenIdsForLocationId(Id locationId) const;

	virtual std::vector<Id> getTokenIdsForQuery(std::string query) const;
	virtual Id getTokenIdForFileNode(const FilePath& filePath) const;

	virtual TokenLocationCollection getTokenLocationsForTokenIds(const std::vector<Id>& tokenIds) const;
	virtual std::shared_ptr<TokenLocationFile> getTokenLocationsForFile(const std::string& filePath) const;
	virtual std::shared_ptr<TokenLocationFile> getTokenLocationsForLinesInFile(
		const std::string& filePath, uint firstLineNumber, uint lastLineNumber
	) const;

	virtual TokenLocationCollection getErrorTokenLocations(std::vector<std::string>* errorMessages) const;

	virtual std::shared_ptr<TokenLocationFile> getTokenLocationOfParentScope(const TokenLocation* child) const;

protected:
	const Graph& getGraph() const;
	const TokenLocationCollection& getTokenLocationCollection() const;
	const SearchIndex& getSearchIndex() const;

private:
	Node* addNodeHierarchy(Node::NodeType type, NameHierarchy nameHierarchy);
	Node* addNodeHierarchyWithDistinctSignature(Node::NodeType type, const ParseFunction& function);

	Node* addFileNode(const FilePath& filePath);
	Node* findFileNode(const FilePath& filePath) const;

	TokenComponentAccess::AccessType convertAccessType(ParserClient::AccessType access) const;
	TokenComponentAccess* addAccess(Node* node, ParserClient::AccessType access);

	TokenComponentAbstraction::AbstractionType convertAbstractionType(ParserClient::AbstractionType abstraction) const;
	TokenComponentAbstraction* addAbstraction(Node* node, ParserClient::AbstractionType abstraction);

	Edge* addTypeEdge(Node* node, Edge::EdgeType edgeType, const ParseTypeUsage& typeUsage);
	TokenLocation* addTokenLocation(Token* token, const ParseLocation& location, bool isScope = false);

	bool getQuerySearchResults(const std::string& query, const std::string& word, SearchResults* results) const;

	void addDependingFilePathsAndRemoveFileNodesRecursive(Node* fileNode, std::set<FilePath>* filePaths);
	void removeNodeIfUnreferenced(Node* node);

	void log(std::string type, std::string str, const ParseLocation& location) const;

	StorageGraph m_graph;
	TokenLocationCollection m_locationCollection;

	SearchIndex m_tokenIndex;
	SearchIndex m_filterIndex;

	TokenLocationCollection m_errorLocationCollection;
	std::vector<std::string> m_errorMessages;
};

#endif // STORAGE_H
