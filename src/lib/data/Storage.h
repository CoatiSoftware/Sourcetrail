#ifndef STORAGE_H
#define STORAGE_H

#include <memory>
#include <vector>

#include "data/access/GraphAccess.h"
#include "data/access/LocationAccess.h"
#include "data/graph/Graph.h"
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
	virtual void onTypedefParsed(
		const ParseLocation& location, const std::string& fullName, const DataType& underlyingType,
		AccessType access
	);
	virtual void onClassParsed(
		const ParseLocation& location, const std::string& fullName, AccessType access, const ParseLocation& scopeLocation);
	virtual void onStructParsed(
		const ParseLocation& location, const std::string& fullName, AccessType access, const ParseLocation& scopeLocation);

	virtual void onGlobalVariableParsed(const ParseLocation& location, const ParseVariable& variable);
	virtual void onFieldParsed(const ParseLocation& location, const ParseVariable& variable, AccessType access);

	virtual void onFunctionParsed(
		const ParseLocation& location, const std::string& fullName, const ParseTypeUsage& returnType,
		const std::vector<ParseTypeUsage>& parameters, const ParseLocation& scopeLocation
	);
	virtual void onMethodParsed(
		const ParseLocation& location, const std::string& fullName, const ParseTypeUsage& returnType,
		const std::vector<ParseTypeUsage>& parameters, AccessType access, AbstractionType abstraction,
		bool isConst, bool isStatic, const ParseLocation& scopeLocation
	);

	virtual void onNamespaceParsed(
		const ParseLocation& location, const std::string& fullName, const ParseLocation& scopeLocation);

	virtual void onEnumParsed(
		const ParseLocation& location, const std::string& fullName, AccessType access, const ParseLocation& scopeLocation);
	virtual void onEnumFieldParsed(const ParseLocation& location, const std::string& fullName);

	virtual void onInheritanceParsed(
		const ParseLocation& location, const std::string& fullName, const std::string& baseName, AccessType access
	);
	virtual void onCallParsed(
		const ParseLocation& location, const std::string& callerName, const std::string& calleeName
	);
	virtual void onFieldUsageParsed(
		const ParseLocation& location, const std::string& userName, const std::string& usedName
	);
	virtual void onGlobalVariableUsageParsed(
		const ParseLocation& location, const std::string& userName, const std::string& usedName);

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

	// LocationAccess implementation
	virtual TokenLocationCollection getTokenLocationsForLocationIds(const std::vector<Id>& locationIds) const;
	virtual TokenLocationFile getTokenLocationsForFile(const std::string& filePath) const;
	virtual TokenLocationFile getTokenLocationsForLinesInFile(
		const std::string& filePath, uint firstLineNumber, uint lastLineNumber
	) const;

private:
	TokenComponentAccess::AccessType convertAccessType(ParserClient::AccessType access) const;
	TokenComponentAccess* addAccess(Node* node, ParserClient::AccessType access);
	Edge* addTypeEdge(Node* node, Edge::EdgeType edgeType, const DataType& type);
	Edge* addTypeEdge(Node* node, Edge::EdgeType edgeType, const ParseTypeUsage& typeUsage);
	TokenLocation* addTokenLocation(Token* token, const ParseLocation& location, bool isScope = false);

	void log(std::string type, std::string str, const ParseLocation& location) const;

	std::vector<std::tuple<Id, Id, Id>> getEdgesOfTypeOfNode(const Id id, const Edge::EdgeType type) const;

	Graph m_graph;
	TokenLocationCollection m_locationCollection;
};

#endif // STORAGE_H
