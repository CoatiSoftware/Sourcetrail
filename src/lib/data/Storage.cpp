#include "data/Storage.h"

#include <sstream>

#include "data/location/TokenLocation.h"
#include "data/location/TokenLocationFile.h"
#include "data/location/TokenLocationLine.h"
#include "data/parser/ParseLocation.h"
#include "data/parser/ParseVariable.h"
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
	std::stringstream str;
	str << "\n" << m_graph;
	LOG_INFO(str.str());
}

void Storage::logLocations() const
{
	std::stringstream str;
	str << "\n" << m_locationCollection;
	LOG_INFO(str.str());
}


void Storage::onTypedefParsed(
	const ParseLocation& location, const std::string& fullName, const std::string& underlyingFullName,
	AccessType access
)
{
	log("typedef", fullName + " -> " + underlyingFullName, location);
}

void Storage::onClassParsed(const ParseLocation& location, const std::string& fullName, AccessType access)
{
	log("class", fullName, location);

	Node* node = m_graph.createNodeHierarchy(fullName);
	node->setType(Node::NODE_CLASS);
	node->setAccess(convertAccessType(access));

	addTokenLocation(node, location);
}

void Storage::onStructParsed(const ParseLocation& location, const std::string& fullName, AccessType access)
{
	log("struct", fullName, location);

	Node* node = m_graph.createNodeHierarchy(fullName);
	node->setType(Node::NODE_STRUCT);
	node->setAccess(convertAccessType(access));

	addTokenLocation(node, location);
}

void Storage::onGlobalVariableParsed(const ParseLocation& location, const ParseVariable& variable)
{
	log("global", variable.fullName, location);

	Node* node = m_graph.createNodeHierarchy(variable.fullName);
	node->setType(Node::NODE_GLOBAL_VARIABLE);
	node->setConst(variable.isConst);
	node->setStatic(variable.isStatic);

	m_graph.createEdge(Edge::EDGE_TYPE_OF, node, m_graph.createNodeHierarchy(variable.typeName));

	addTokenLocation(node, location);
}

void Storage::onFieldParsed(const ParseLocation& location, const ParseVariable& variable, AccessType access)
{
	log("field", variable.fullName, location);

	Node* node = m_graph.createNodeHierarchy(variable.fullName);
	node->setType(Node::NODE_FIELD);
	node->setConst(variable.isConst);
	node->setStatic(variable.isStatic);

	if (access == ACCESS_NONE)
	{
		LOG_ERROR("Field needs to have access type [public, protected, private] but has none.");
		return;
	}
	node->setAccess(convertAccessType(access));

	m_graph.createEdge(Edge::EDGE_TYPE_OF, node, m_graph.createNodeHierarchy(variable.typeName));

	addTokenLocation(node, location);
}

void Storage::onFunctionParsed(
	const ParseLocation& location, const std::string& fullName, const std::string& returnTypeName,
	const std::vector<ParseVariable>& parameters
)
{
	log("function", fullName, location);

	Node* node = m_graph.createNodeHierarchy(fullName);
	node->setType(Node::NODE_FUNCTION);

	m_graph.createEdge(Edge::EDGE_RETURN_TYPE_OF, node, m_graph.createNodeHierarchy(returnTypeName));
	for (const ParseVariable& var : parameters)
	{
		m_graph.createEdge(Edge::EDGE_PARAMETER_OF, node, m_graph.createNodeHierarchy(var.typeName));
	}

	addTokenLocation(node, location);
}

void Storage::onMethodParsed(
	const ParseLocation& location, const std::string& fullName, const std::string& returnTypeName,
	const std::vector<ParseVariable>& parameters, AccessType access, AbstractionType abstraction,
	bool isConst, bool isStatic
)
{
	log("method", fullName, location);

	Node* node = m_graph.createNodeHierarchy(fullName);
	node->setType(Node::NODE_METHOD);
	node->setConst(isConst);
	node->setStatic(isStatic);

	if (access == ACCESS_NONE)
	{
		LOG_ERROR("Method needs to have access type [public, protected, private] but has none.");
		return;
	}
	node->setAccess(convertAccessType(access));

	m_graph.createEdge(Edge::EDGE_RETURN_TYPE_OF, node, m_graph.createNodeHierarchy(returnTypeName));
	for (const ParseVariable& parameter : parameters)
	{
		m_graph.createEdge(Edge::EDGE_PARAMETER_OF, node, m_graph.createNodeHierarchy(parameter.typeName));
	}

	addTokenLocation(node, location);
}

void Storage::onNamespaceParsed(const ParseLocation& location, const std::string& fullName)
{
	log("namespace", fullName, location);

	Node* node = m_graph.createNodeHierarchy(fullName);
	node->setType(Node::NODE_NAMESPACE);

	addTokenLocation(node, location);
}

void Storage::onEnumParsed(const ParseLocation& location, const std::string& fullName, AccessType access)
{
	log("enum", fullName, location);

	Node* node = m_graph.createNodeHierarchy(fullName);
	node->setType(Node::NODE_ENUM);
	node->setAccess(convertAccessType(access));

	addTokenLocation(node, location);
}

void Storage::onEnumFieldParsed(const ParseLocation& location, const std::string& fullName)
{
	log("enum field", fullName, location);

	Node* node = m_graph.createNodeHierarchy(fullName);
	node->setType(Node::NODE_FIELD);

	addTokenLocation(node, location);
}

void Storage::onInheritanceParsed(
	const ParseLocation& location, const std::string& fullName, const std::string& baseName, AccessType access)
{
	log("inheritance", fullName + " : " + baseName, location);

	Node* node = m_graph.createNodeHierarchy(fullName);
	Node* baseNode = m_graph.createNodeHierarchy(baseName);

	Edge* edge = m_graph.createEdge(Edge::EDGE_INHERITANCE, node, baseNode);
	edge->setAccess(convertAccessType(access));

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

Edge::AccessType Storage::convertAccessType(ParserClient::AccessType access) const
{
	switch (access)
	{
	case ACCESS_PUBLIC:
		return Edge::ACCESS_PUBLIC;
	case ACCESS_PROTECTED:
		return Edge::ACCESS_PROTECTED;
	case ACCESS_PRIVATE:
		return Edge::ACCESS_PRIVATE;
	case ACCESS_NONE:
		return Edge::ACCESS_NONE;
	}
}

TokenLocation* Storage::addTokenLocation(Token* token, const ParseLocation& loc)
{
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
	std::stringstream info;
	info << type << ": " << str;
	info << " <" << location.filePath << " ";
	info << location.startLineNumber << ":" << location.startColumnNumber << " ";
	info << location.endLineNumber << ":" << location.endColumnNumber << ">";
	LOG_INFO(info.str());
}
