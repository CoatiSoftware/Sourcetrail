#ifndef STORAGE_H
#define STORAGE_H

#include <memory>
#include <vector>

#include "data/access/GraphAccess.h"
#include "data/access/LocationAccess.h"
#include "data/graph/StorageGraph.h"
#include "data/graph/token_component/TokenComponentAbstraction.h"
#include "data/graph/token_component/TokenComponentAccess.h"
#include "data/location/TokenLocationCollection.h"
#include "data/parser/ParserClient.h"
#include "data/search/SearchIndex.h"

class Storage
	: public ParserClient
	, public GraphAccess
	, public LocationAccess
{
public:
	Storage();
	virtual ~Storage();

	void clear();
	void clearFileData(const std::vector<std::string>& filePaths);

	void logGraph() const;
	void logLocations() const;

	size_t getErrorCount() const;

	// ParserClient implementation
	virtual void onError(const ParseLocation& location, const std::string& message);

	virtual Id onTypedefParsed(
		const ParseLocation& location, const std::vector<std::string>& nameHierarchy,
		const ParseTypeUsage& underlyingType, AccessType access);
	virtual Id onClassParsed(
		const ParseLocation& location, const std::vector<std::string>& nameHierarchy, AccessType access,
		const ParseLocation& scopeLocation);
	virtual Id onStructParsed(
		const ParseLocation& location, const std::vector<std::string>& nameHierarchy, AccessType access,
		const ParseLocation& scopeLocation);

	virtual Id onGlobalVariableParsed(const ParseLocation& location, const ParseVariable& variable);
	virtual Id onFieldParsed(const ParseLocation& location, const ParseVariable& variable, AccessType access);

	virtual Id onFunctionParsed(
		const ParseLocation& location, const ParseFunction& function, const ParseLocation& scopeLocation);
	virtual Id onMethodParsed(
		const ParseLocation& location, const ParseFunction& method, AccessType access, AbstractionType abstraction,
		const ParseLocation& scopeLocation);

	virtual Id onNamespaceParsed(
		const ParseLocation& location, const std::vector<std::string>& nameHierarchy, const ParseLocation& scopeLocation);

	virtual Id onEnumParsed(
		const ParseLocation& location, const std::vector<std::string>& nameHierarchy, AccessType access,
		const ParseLocation& scopeLocation);
	virtual Id onEnumFieldParsed(const ParseLocation& location, const std::vector<std::string>& nameHierarchy);

	virtual Id onInheritanceParsed(
		const ParseLocation& location, const std::vector<std::string>& nameHierarchy,
		const std::vector<std::string>& baseNameHierarchy, AccessType access);
	virtual Id onCallParsed(
		const ParseLocation& location, const ParseFunction& caller, const ParseFunction& callee);
	virtual Id onCallParsed(
		const ParseLocation& location, const ParseVariable& caller, const ParseFunction& callee);
	Id onVariableUsageParsed(
		const std::string kind, const ParseLocation& location, const ParseFunction& user,
		const std::vector<std::string>& usedNameHierarchy); // helper
	virtual Id onFieldUsageParsed(
		const ParseLocation& location, const ParseFunction& user, const std::vector<std::string>& usedNameHierarchy);
	virtual Id onGlobalVariableUsageParsed(
		const ParseLocation& location, const ParseFunction& user, const std::vector<std::string>& usedNameHierarchy);
	virtual Id onEnumFieldUsageParsed(
		const ParseLocation& location, const ParseFunction& user, const std::vector<std::string>& usedNameHierarchy);
	virtual Id onEnumFieldUsageParsed(
		const ParseLocation& location, const ParseVariable& user, const std::vector<std::string>& usedNameHierarchy);
	virtual Id onTypeUsageParsed(const ParseTypeUsage& type, const ParseFunction& function);
	virtual Id onTypeUsageParsed(const ParseTypeUsage& type, const ParseVariable& variable);

	virtual Id onTemplateRecordParameterTypeParsed(
		const ParseLocation& location, const std::string& templateParameterTypeName,
		const std::vector<std::string>& templateRecordNameHierarchy);
	virtual Id onTemplateRecordArgumentTypeParsed(
		const ParseLocation& location, const std::vector<std::string>& templateArgumentTypeNameHierarchy,
		const std::vector<std::string>& templateRecordNameHierarchy);
	virtual Id onTemplateDefaultArgumentTypeParsed(
		const ParseTypeUsage& type,	const std::vector<std::string>& templateArgumentTypeNameHierarchy);

	virtual Id onTemplateRecordSpecializationParsed(
		const ParseLocation& location, const std::vector<std::string>& specializedRecordNameHierarchy,
		const RecordType specializedRecordType, const std::vector<std::string>& specializedFromNameHierarchy);

	virtual Id onTemplateFunctionParameterTypeParsed(
		const ParseLocation& location, const std::string& templateParameterTypeName, const ParseFunction function);
	virtual Id onTemplateFunctionSpecializationParsed(
		const ParseLocation& location, const ParseFunction specializedFunction, const ParseFunction templateFunction);

	// GraphAccess implementation
	virtual Id getIdForNodeWithName(const std::string& fullName) const;
	virtual std::string getNameForNodeWithId(Id id) const;
	virtual std::vector<SearchMatch> getAutocompletionMatches(
		const std::string& query, const std::string& word) const;

	virtual std::shared_ptr<Graph> getGraphForActiveTokenIds(const std::vector<Id>& tokenIds) const;

	virtual std::vector<Id> getActiveTokenIdsForId(Id tokenId, Id* declarationId) const;
	virtual std::vector<Id> getActiveTokenIdsForLocationId(Id locationId) const;
	virtual std::vector<Id> getLocationIdsForTokenIds(const std::vector<Id>& tokenIds) const;

	virtual std::vector<Id> getTokenIdsForQuery(std::string query) const;

	// LocationAccess implementation
	virtual TokenLocationCollection getTokenLocationsForLocationIds(const std::vector<Id>& locationIds) const;
	virtual TokenLocationFile getTokenLocationsForFile(const std::string& filePath) const;
	virtual TokenLocationFile getTokenLocationsForLinesInFile(
		const std::string& filePath, uint firstLineNumber, uint lastLineNumber
	) const;

	virtual TokenLocationCollection getErrorTokenLocations(std::vector<std::string>* errorMessages) const;

protected:
	const Graph& getGraph() const;
	const TokenLocationCollection& getTokenLocationCollection() const;
	const SearchIndex& getSearchIndex() const;

private:
	Node* addNodeHierarchy(Node::NodeType type, std::vector<std::string> nameHierarchy);
	Node* addNodeHierarchyWithDistinctSignature(Node::NodeType type, const ParseFunction& function);

	TokenComponentAccess::AccessType convertAccessType(ParserClient::AccessType access) const;
	TokenComponentAccess* addAccess(Node* node, ParserClient::AccessType access);

	TokenComponentAbstraction::AbstractionType convertAbstractionType(ParserClient::AbstractionType abstraction) const;
	TokenComponentAbstraction* addAbstraction(Node* node, ParserClient::AbstractionType abstraction);

	Edge* addTypeEdge(Node* node, Edge::EdgeType edgeType, const ParseTypeUsage& typeUsage);
	TokenLocation* addTokenLocation(Token* token, const ParseLocation& location, bool isScope = false);

	bool getQuerySearchResults(const std::string& query, const std::string& word, SearchResults* results) const;

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
