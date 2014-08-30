#ifndef STORAGE_H
#define STORAGE_H

#include <memory>
#include <vector>

#include "data/access/GraphAccess.h"
#include "data/access/LocationAccess.h"
#include "data/graph/Graph.h"
#include "data/graph/token_component/TokenComponentAbstraction.h"
#include "data/graph/token_component/TokenComponentAccess.h"
#include "data/location/TokenLocationCollection.h"
#include "data/parser/ParserClient.h"

class Storage
	: public ParserClient
	, public GraphAccess
	, public LocationAccess
{
public:
	Storage();
	virtual ~Storage();

	void clear();

	void logGraph() const;
	void logLocations() const;

	// ParserClient implementation
	virtual Id onTypedefParsed(
		const ParseLocation& location, const std::string& fullName, const ParseTypeUsage& underlyingType,
		AccessType access);
	virtual Id onClassParsed(
		const ParseLocation& location, const std::string& fullName, AccessType access, const ParseLocation& scopeLocation);
	virtual Id onStructParsed(
		const ParseLocation& location, const std::string& fullName, AccessType access, const ParseLocation& scopeLocation);

	virtual Id onGlobalVariableParsed(const ParseLocation& location, const ParseVariable& variable);
	virtual Id onFieldParsed(const ParseLocation& location, const ParseVariable& variable, AccessType access);

	virtual Id onFunctionParsed(
		const ParseLocation& location, const ParseFunction& function, const ParseLocation& scopeLocation);
	virtual Id onMethodParsed(
		const ParseLocation& location, const ParseFunction& method, AccessType access, AbstractionType abstraction,
		const ParseLocation& scopeLocation);

	virtual Id onNamespaceParsed(
		const ParseLocation& location, const std::string& fullName, const ParseLocation& scopeLocation);

	virtual Id onEnumParsed(
		const ParseLocation& location, const std::string& fullName, AccessType access, const ParseLocation& scopeLocation);
	virtual Id onEnumFieldParsed(const ParseLocation& location, const std::string& fullName);

	virtual Id onInheritanceParsed(
		const ParseLocation& location, const std::string& fullName, const std::string& baseName, AccessType access);
	virtual Id onCallParsed(
		const ParseLocation& location, const ParseFunction& caller, const ParseFunction& callee);
	virtual Id onCallParsed(
		const ParseLocation& location, const ParseVariable& caller, const ParseFunction& callee);
	virtual Id onFieldUsageParsed(
		const ParseLocation& location, const ParseFunction& user, const std::string& usedName);
	virtual Id onGlobalVariableUsageParsed(
		const ParseLocation& location, const ParseFunction& user, const std::string& usedName);
	virtual Id onTypeUsageParsed(const ParseTypeUsage& type, const ParseFunction& function);

	// GraphAccess implementation
	virtual Id getIdForNodeWithName(const std::string& fullName) const;
	virtual std::string getNameForNodeWithId(Id id) const;
	virtual std::vector<std::string> getNamesForNodesWithNamePrefix(const std::string& prefix) const;
	virtual std::vector<Id> getIdsOfNeighbours(const Id id) const;
	virtual std::vector<std::tuple<Id, Id, Id>> getNeighbourEdgesOfNode(const Id id) const;
	virtual std::vector<std::tuple<Id, Id, Id>> getMemberEdgesOfNode(const Id id) const;
	virtual std::vector<std::tuple<Id, Id, Id>> getUsageEdgesOfNode(const Id id) const;
	virtual std::vector<std::tuple<Id, Id, Id>> getCallEdgesOfNode(const Id id) const;
	virtual std::vector<std::tuple<Id, Id, Id>> getTypeOfEdgesOfNode(const Id id) const;
	virtual std::vector<std::tuple<Id, Id, Id>> getReturnTypeOfEdgesOfNode(const Id id) const;
	virtual std::vector<std::tuple<Id, Id, Id>> getParameterOfEdgesOfNode(const Id id) const;
	virtual std::vector<std::tuple<Id, Id, Id>> getInheritanceEdgesOfNode(const Id id) const;

	virtual std::pair<Id, Id> getNodesOfEdge(const Id id) const;

	virtual bool checkTokenIsNode(const Id id) const;

	virtual std::vector<Id> getActiveTokenIdsForId(Id tokenId) const;
	virtual std::vector<Id> getLocationIdsForTokenIds(const std::vector<Id>& tokenIds) const;

	virtual std::vector<Id> getTokenIdsForQuery(std::string query) const;

	// LocationAccess implementation
	virtual TokenLocationCollection getTokenLocationsForLocationIds(const std::vector<Id>& locationIds) const;
	virtual TokenLocationFile getTokenLocationsForFile(const std::string& filePath) const;
	virtual TokenLocationFile getTokenLocationsForLinesInFile(
		const std::string& filePath, uint firstLineNumber, uint lastLineNumber
	) const;

protected:
	Token* getTokenWithId(Id tokenId) const;
	std::vector<TokenLocation*> getTokenLocationsForId(Id tokenId) const;

private:
	TokenComponentAccess::AccessType convertAccessType(ParserClient::AccessType access) const;
	TokenComponentAccess* addAccess(Node* node, ParserClient::AccessType access);

	TokenComponentAbstraction::AbstractionType convertAbstractionType(ParserClient::AbstractionType abstraction) const;
	TokenComponentAbstraction* addAbstraction(Node* node, ParserClient::AbstractionType abstraction);

	Edge* addTypeEdge(Node* node, Edge::EdgeType edgeType, const DataType& type);
	Edge* addTypeEdge(Node* node, Edge::EdgeType edgeType, const ParseTypeUsage& typeUsage);
	TokenLocation* addTokenLocation(Token* token, const ParseLocation& location, bool isScope = false);

	void log(std::string type, std::string str, const ParseLocation& location) const;

	std::vector<std::tuple<Id, Id, Id>> getEdgesOfTypeOfNode(const Id id, const Edge::EdgeType type) const;

	Graph m_graph;
	TokenLocationCollection m_locationCollection;
};

#endif // STORAGE_H
