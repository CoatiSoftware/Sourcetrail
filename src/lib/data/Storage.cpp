#include "data/Storage.h"

#include "data/graph/token_component/TokenComponentConst.h"
#include "data/graph/token_component/TokenComponentDataType.h"
#include "data/graph/token_component/TokenComponentStatic.h"
#include "data/location/TokenLocation.h"
#include "data/location/TokenLocationFile.h"
#include "data/location/TokenLocationLine.h"
#include "data/parser/ParseLocation.h"
#include "data/parser/ParseTypeUsage.h"
#include "data/parser/ParseVariable.h"
#include "data/type/DataType.h"
#include "utility/logging/logging.h"
#include "utility/utilityString.h"

Storage::Storage()
{
}

Storage::~Storage()
{
}

void Storage::clear()
{
	m_graph.clear();
	m_locationCollection.clear();
}

void Storage::logGraph() const
{
	LOG_INFO_STREAM(<< '\n' << m_graph);
}

void Storage::logLocations() const
{
	LOG_INFO_STREAM(<< '\n' << m_locationCollection);
}


void Storage::onTypedefParsed(
	const ParseLocation& location, const std::string& fullName, const DataType& underlyingType, AccessType access
){
	log("typedef", fullName + " -> " + underlyingType.getFullTypeName(), location);

	Node* node = m_graph.createNodeHierarchy(Node::NODE_TYPEDEF, fullName);
	addAccess(node, access);
	addTokenLocation(node, location);
	addTypeEdge(node, Edge::EDGE_TYPEDEF_OF, underlyingType);
}

void Storage::onClassParsed(const ParseLocation& location, const std::string& fullName, AccessType access)
{
	log("class", fullName, location);

	Node* node = m_graph.createNodeHierarchy(Node::NODE_CLASS, fullName);
	addAccess(node, access);
	addTokenLocation(node, location);
}

void Storage::onStructParsed(const ParseLocation& location, const std::string& fullName, AccessType access)
{
	log("struct", fullName, location);

	Node* node = m_graph.createNodeHierarchy(Node::NODE_STRUCT, fullName);
	addAccess(node, access);
	addTokenLocation(node, location);
}

void Storage::onGlobalVariableParsed(const ParseLocation& location, const ParseVariable& variable)
{
	log("global", variable.fullName, location);

	Node* node = m_graph.createNodeHierarchy(Node::NODE_GLOBAL_VARIABLE, variable.fullName);

	if (variable.isStatic)
	{
		node->addComponentStatic(std::make_shared<TokenComponentStatic>());
	}

	addTypeEdge(node, Edge::EDGE_TYPE_OF, variable.type);
	addTokenLocation(node, location);
}

void Storage::onFieldParsed(const ParseLocation& location, const ParseVariable& variable, AccessType access)
{
	log("field", variable.fullName, location);

	Node* node = m_graph.createNodeHierarchy(Node::NODE_FIELD, variable.fullName);

	if (variable.isStatic)
	{
		node->addComponentStatic(std::make_shared<TokenComponentStatic>());
	}

	if (access == ACCESS_NONE)
	{
		LOG_ERROR("Field needs to have access type [public, protected, private] but has none.");
		return;
	}
	addAccess(node, access);

	addTypeEdge(node, Edge::EDGE_TYPE_OF, variable.type);
	addTokenLocation(node, location);
}

void Storage::onFunctionParsed(
	const ParseLocation& location, const std::string& fullName, const ParseTypeUsage& returnType,
	const std::vector<ParseTypeUsage>& parameters
)
{
	log("function", fullName, location);

	Node* node = m_graph.createNodeHierarchyWithDistinctSignature(
		Node::NODE_FUNCTION, fullName,
		ParserClient::functionSignatureStr(returnType.type, fullName, parameters, false)
	);
	addTokenLocation(node, location);

	addTypeEdge(node, Edge::EDGE_RETURN_TYPE_OF, returnType);
	for (const ParseTypeUsage& parameter : parameters)
	{
		addTypeEdge(node, Edge::EDGE_PARAMETER_TYPE_OF, parameter);
	}
}

void Storage::onMethodParsed(
	const ParseLocation& location, const std::string& fullName, const ParseTypeUsage& returnType,
	const std::vector<ParseTypeUsage>& parameters, AccessType access, AbstractionType abstraction,
	bool isConst, bool isStatic
)
{
	log("method", fullName, location);

	Node* node = m_graph.createNodeHierarchyWithDistinctSignature(
		Node::NODE_METHOD, fullName,
		ParserClient::functionSignatureStr(returnType.type, fullName, parameters, isConst)
	);

	if (isConst)
	{
		node->addComponentConst(std::make_shared<TokenComponentConst>());
	}

	if (isStatic)
	{
		node->addComponentStatic(std::make_shared<TokenComponentStatic>());
	}

	if (access == ACCESS_NONE)
	{
		LOG_ERROR("Method needs to have access type [public, protected, private] but has none.");
		return;
	}
	addAccess(node, access);

	addTokenLocation(node, location);

	addTypeEdge(node, Edge::EDGE_RETURN_TYPE_OF, returnType);
	for (const ParseTypeUsage& parameter : parameters)
	{
		addTypeEdge(node, Edge::EDGE_PARAMETER_TYPE_OF, parameter);
	}
}

void Storage::onNamespaceParsed(const ParseLocation& location, const std::string& fullName)
{
	log("namespace", fullName, location);

	Node* node = m_graph.createNodeHierarchy(Node::NODE_NAMESPACE, fullName);
	addTokenLocation(node, location);
}

void Storage::onEnumParsed(const ParseLocation& location, const std::string& fullName, AccessType access)
{
	log("enum", fullName, location);

	Node* node = m_graph.createNodeHierarchy(Node::NODE_ENUM, fullName);
	addAccess(node, access);
	addTokenLocation(node, location);
}

void Storage::onEnumFieldParsed(const ParseLocation& location, const std::string& fullName)
{
	log("enum field", fullName, location);

	Node* node = m_graph.createNodeHierarchy(Node::NODE_FIELD, fullName);
	addTokenLocation(node, location);
}

void Storage::onInheritanceParsed(
	const ParseLocation& location, const std::string& fullName, const std::string& baseName, AccessType access)
{
	log("inheritance", fullName + " : " + baseName, location);

	Node* node = m_graph.createNodeHierarchy(fullName);
	Node* baseNode = m_graph.createNodeHierarchy(baseName);

	Edge* edge = m_graph.createEdge(Edge::EDGE_INHERITANCE, node, baseNode);
	edge->addComponentAccess(std::make_shared<TokenComponentAccess>(convertAccessType(access)));

	addTokenLocation(edge, location);
}

void Storage::onCallParsed(const ParseLocation& location, const std::string& callerName, const std::string& calleeName)
{
	log("call", callerName + " -> " + calleeName, location);

	Node* callerNode = m_graph.createNodeHierarchy(callerName);
	Node* calleeNode = m_graph.createNodeHierarchy(calleeName);

	Edge* edge = m_graph.createEdge(Edge::EDGE_CALL, callerNode, calleeNode);

	addTokenLocation(edge, location);
}

void Storage::onFieldUsageParsed(const ParseLocation& location, const std::string& userName, const std::string& usedName)
{
	log("usage", userName + " -> " + usedName, location);

	Node* userNode = m_graph.createNodeHierarchy(userName);
	Node* usedNode = m_graph.createNodeHierarchy(usedName);

	Edge* edge = m_graph.createEdge(Edge::EDGE_USAGE, userNode, usedNode);
	addTokenLocation(edge, location);
}

void Storage::onGlobalVariableUsageParsed(
	const ParseLocation& location, const std::string& userName, const std::string& usedName
)
{
	log("usage", userName + " -> " + usedName, location);

	Node* userNode = m_graph.createNodeHierarchy(userName);
	Node* usedNode = m_graph.createNodeHierarchy(usedName);

	Edge* edge = m_graph.createEdge(Edge::EDGE_USAGE, userNode, usedNode);
	addTokenLocation(edge, location);
}

Id Storage::getIdForNodeWithName(const std::string& name) const
{
	Node* node = m_graph.findNode([&](Node* node){
		return node->getName() == name;
	});

	return (node ? node->getId() : 0);
}

std::string Storage::getNameForNodeWithId(Id id) const
{
	Node* node = m_graph.getNodeById(id);
	return (node ? node->getName() : "");
}

std::vector<std::string> Storage::getNamesForNodesWithNamePrefix(const std::string& prefix) const
{
	std::vector<std::string> names;
	m_graph.forEachNode([&](Node* node){
		const std::string& nodeName = node->getName();
		if (utility::isPrefix(prefix, nodeName))
		{
			names.push_back(nodeName);
		}
	});

	return names;
}

std::vector<Id> Storage::getIdsOfNeighbours(const Id id) const
{
	std::vector<Id> result;

	Node* node = m_graph.findNode([&](Node* node){
		return node->getId() == id;
	});

	if (node != NULL)
	{
		std::map<Id, bool> addedIds;
		addedIds[id] = true;

		node->forEachEdge(
				[&result, &addedIds](Edge* e)
				{
					Id fromId = e->getFrom()->getId();
					if (addedIds.find(fromId) == addedIds.end())
					{
						result.push_back(fromId);
						addedIds[fromId] = true;
					}

					Id toId = e->getTo()->getId();
					if (addedIds.find(toId) == addedIds.end())
					{
						result.push_back(toId);
						addedIds[toId] = true;
					}
				}
			);
	}

	return result;
}

std::vector<std::tuple<Id, Id, Id>> Storage::getNeighbourEdgesOfNode(const Id id) const
{
	std::vector<std::tuple<Id, Id, Id>> result;

	Node* node = m_graph.findNode([&](Node* node){
		return node->getId() == id;
	});

	if (node != NULL)
	{
		node->forEachEdge(
				[&result](Edge* e)
				{
					result.push_back(std::tuple<Id, Id, Id>(e->getFrom()->getId(), e->getTo()->getId(), e->getId()));
				}
			);
	}

	return result;
}

std::vector<std::tuple<Id, Id, Id>> Storage::getMemberEdgesOfNode(const Id id) const
{
	return getEdgesOfTypeOfNode(id, Edge::EdgeType::EDGE_MEMBER);
}

std::vector<std::tuple<Id, Id, Id>> Storage::getUsageEdgesOfNode(const Id id) const
{
	return getEdgesOfTypeOfNode(id, Edge::EdgeType::EDGE_USAGE);
}

std::vector<std::tuple<Id, Id, Id>> Storage::getCallEdgesOfNode(const Id id) const
{
	return getEdgesOfTypeOfNode(id, Edge::EdgeType::EDGE_CALL);
}

std::vector<std::tuple<Id, Id, Id>> Storage::getTypeOfEdgesOfNode(const Id id) const
{
	return getEdgesOfTypeOfNode(id, Edge::EdgeType::EDGE_TYPE_OF);
}

std::vector<std::tuple<Id, Id, Id>> Storage::getReturnTypeOfEdgesOfNode(const Id id) const
{
	return getEdgesOfTypeOfNode(id, Edge::EdgeType::EDGE_RETURN_TYPE_OF);
}

std::vector<std::tuple<Id, Id, Id>> Storage::getParameterOfEdgesOfNode(const Id id) const
{
	return getEdgesOfTypeOfNode(id, Edge::EdgeType::EDGE_PARAMETER_TYPE_OF);
}

std::vector<std::tuple<Id, Id, Id>> Storage::getInheritanceEdgesOfNode(const Id id) const
{
	return getEdgesOfTypeOfNode(id, Edge::EdgeType::EDGE_INHERITANCE);
}

std::pair<Id, Id> Storage::getNodesOfEdge(const Id id) const
{
	std::pair<Id, Id> result;

	Edge* edge = m_graph.findEdge([&](Edge* edge){
		return edge->getId() == id;
	});

	if(edge != NULL)
	{
		result.first = edge->getFrom()->getId();
		result.second = edge->getTo()->getId();
	}

	return result;
}

bool Storage::checkTokenIsNode(const Id id) const
{
	Token* token = m_graph.findToken([&](Token* token){
		return token->getId() == id;
	});

	if(token != NULL)
	{
		return token->isNode();
	}

	return false;
}

TokenLocationCollection Storage::getTokenLocationsForTokenId(Id id) const
{
	TokenLocationCollection ret;

	Token* token = m_graph.getTokenById(id);
	if (!token)
	{
		return ret;
	}

	std::vector<Id> locationIds = token->getLocationIds();
	for (Id locationId: locationIds)
	{
		TokenLocation* location = m_locationCollection.findTokenLocationById(locationId);
		if (location->getOtherTokenLocation())
		{
			ret.addTokenLocationAsPlainCopy(location);
			ret.addTokenLocationAsPlainCopy(location->getOtherTokenLocation());
		}
	}

	return ret;
}

TokenLocationFile Storage::getTokenLocationsForLinesInFile(
		const std::string& fileName, unsigned int firstLineNumber, unsigned int lastLineNumber
) const
{
	TokenLocationFile ret(fileName);

	TokenLocationFile* locationFile = m_locationCollection.findTokenLocationFileByPath(fileName);
	if (!locationFile)
	{
		return ret;
	}

	for (unsigned int i = firstLineNumber; i <= lastLineNumber; i++)
	{
		TokenLocationLine* locationLine = locationFile->findTokenLocationLineByNumber(i);
		if (!locationLine)
		{
			continue;
		}

		locationLine->forEachTokenLocation(
			[&](TokenLocation* tokenLocation) -> void
			{
				ret.addTokenLocationAsPlainCopy(tokenLocation);
			}
		);
	}

	return ret;
}

TokenComponentAccess::AccessType Storage::convertAccessType(ParserClient::AccessType access) const
{
	switch (access)
	{
	case ACCESS_PUBLIC:
		return TokenComponentAccess::ACCESS_PUBLIC;
	case ACCESS_PROTECTED:
		return TokenComponentAccess::ACCESS_PROTECTED;
	case ACCESS_PRIVATE:
		return TokenComponentAccess::ACCESS_PRIVATE;
	case ACCESS_NONE:
		return TokenComponentAccess::ACCESS_NONE;
	}
}

TokenComponentAccess* Storage::addAccess(Node* node, ParserClient::AccessType access)
{
	if (access != ACCESS_NONE)
	{
		std::shared_ptr<TokenComponentAccess> ptr = std::make_shared<TokenComponentAccess>(convertAccessType(access));
		node->getMemberEdge()->addComponentAccess(ptr);
		return ptr.get();
	}
	return nullptr;
}

Edge* Storage::addTypeEdge(Node* node, Edge::EdgeType edgeType, const DataType& type)
{
	Node* typeNode = m_graph.createNodeHierarchy(type.getRawTypeName());
	Edge* edge = m_graph.createEdge(edgeType, node, typeNode);

	// FIXME: When a function uses the same type multiple times then we still only use one edge to save this,
	// but we can't store multiple DataTypes on this edge at the moment.
	if (!edge->getComponent<TokenComponentDataType>())
	{
		edge->addComponentDataType(
			std::make_shared<TokenComponentDataType>(type.getQualifierList(), type.getModifierStack())
		);
	}

	return edge;
}

Edge* Storage::addTypeEdge(Node* node, Edge::EdgeType edgeType, const ParseTypeUsage& typeUsage)
{
	if (!typeUsage.location.isValid())
	{
		return nullptr;
	}

	Edge* edge = addTypeEdge(node, edgeType, typeUsage.type);

	addTokenLocation(edge, typeUsage.location);
	return edge;
}

TokenLocation* Storage::addTokenLocation(Token* token, const ParseLocation& loc)
{
	if (!loc.isValid())
	{
		return nullptr;
	}

	TokenLocation* location = m_locationCollection.addTokenLocation(
		token->getId(), loc.filePath,
		loc.startLineNumber, loc.startColumnNumber,
		loc.endLineNumber, loc.endColumnNumber
	);

	token->addLocationId(location->getId());
	return location;
}

void Storage::log(std::string type, std::string str, const ParseLocation& location) const
{
	LOG_INFO_STREAM(
		<< type << ": " << str << " <" << location.filePath << " "
		<< location.startLineNumber << ":" << location.startColumnNumber << " "
		<< location.endLineNumber << ":" << location.endColumnNumber << ">"
	);
}

std::vector<std::tuple<Id, Id, Id>> Storage::getEdgesOfTypeOfNode(const Id id, const Edge::EdgeType type) const
{
	std::vector<std::tuple<Id, Id, Id>> result;

	Node* node = m_graph.findNode([&](Node* node){
		return node->getId() == id;
	});

	if (node != NULL)
	{
		node->forEachEdge(
				[&result, &type](Edge* e)
				{
					if(e->getType() == type)
					{
						result.push_back(std::tuple<Id, Id, Id>(e->getFrom()->getId(), e->getTo()->getId(), e->getId()));
					}
				}
			);
	}

	return result;
}
