#include "data/Storage.h"

#include "utility/logging/logging.h"
#include "utility/utilityString.h"

#include "data/graph/filter/GraphFilterConductor.h"
#include "data/graph/token_component/TokenComponentConst.h"
#include "data/graph/token_component/TokenComponentName.h"
#include "data/graph/token_component/TokenComponentStatic.h"
#include "data/graph/SubGraph.h"
#include "data/location/TokenLocation.h"
#include "data/location/TokenLocationFile.h"
#include "data/location/TokenLocationLine.h"
#include "data/parser/ParseFunction.h"
#include "data/parser/ParseLocation.h"
#include "data/parser/ParseTypeUsage.h"
#include "data/parser/ParseVariable.h"
#include "data/query/QueryCommand.h"
#include "data/query/QueryTree.h"
#include "data/type/DataType.h"

Storage::Storage()
{
	for (const std::pair<std::string, QueryCommand::CommandType>& p : QueryCommand::getCommandTypeMap())
	{
		m_filterIndex.addNode(std::vector<std::string>(1, p.first));
	}
}

Storage::~Storage()
{
}

void Storage::clear()
{
	m_graph.clear();
	m_locationCollection.clear();
	m_tokenIndex.clear();

	m_errorMessages.clear();
	m_errorLocationCollection.clear();
}

void Storage::clearFileData(const std::vector<std::string>& filePaths)
{
	for (const std::string& filePath : filePaths)
	{
		TokenLocationFile* errorFile = m_errorLocationCollection.findTokenLocationFileByPath(filePath);
		if (errorFile)
		{
			m_errorLocationCollection.removeTokenLocationFile(errorFile);
		}

		TokenLocationFile* file = m_locationCollection.findTokenLocationFileByPath(filePath);
		if (!file)
		{
			continue;
		}

		file->forEachTokenLocation(
			[&](TokenLocation* location)
			{
				if (location->isEndTokenLocation())
				{
					return;
				}

				Token* token = m_graph.getTokenById(location->getTokenId());
				if (!token)
				{
					return;
				}

				token->removeLocationId(location->getId());
				if (token->getLocationIds().size())
				{
					return;
				}

				if (token->isEdge())
				{
					Edge* edge = dynamic_cast<Edge*>(token);
					Node* from = edge->getFrom();
					Node* to = edge->getTo();

					m_graph.removeEdge(edge);
					removeNodeIfUnreferenced(from);
					removeNodeIfUnreferenced(to);
				}
				else
				{
					removeNodeIfUnreferenced(dynamic_cast<Node*>(token));
				}
			}
		);

		m_locationCollection.removeTokenLocationFile(file);
	}
}

void Storage::logGraph() const
{
	LOG_INFO_STREAM(<< '\n' << m_graph);
}

void Storage::logLocations() const
{
	LOG_INFO_STREAM(<< '\n' << m_locationCollection);
}

size_t Storage::getErrorCount() const
{
	return m_errorLocationCollection.getTokenLocationCount();
}

void Storage::onError(const ParseLocation& location, const std::string& message)
{
	log("ERROR", message, location);

	if (!location.isValid())
	{
		return;
	}

	bool duplicate = false;
	std::string filePath = location.filePath;
	TokenLocationFile* file = m_errorLocationCollection.findTokenLocationFileByPath(filePath);

	if (file)
	{
		file->forEachTokenLocation(
			[&](TokenLocation* loc)
			{
				if (loc->isStartTokenLocation() &&
					loc->getLineNumber() == location.startLineNumber &&
					loc->getColumnNumber() == location.startColumnNumber &&
					m_errorMessages[loc->getTokenId()] == message)
				{
					duplicate = true;
				}
			}
		);
	}

	if (!duplicate)
	{
		Id errorId = m_errorMessages.size();

		TokenLocation* loc = m_errorLocationCollection.addTokenLocation(
			errorId, filePath,
			location.startLineNumber, location.startColumnNumber,
			location.endLineNumber, location.endColumnNumber
		);

		m_errorMessages.push_back(message);
	}
}

Id Storage::onTypedefParsed(
	const ParseLocation& location, const std::vector<std::string>& nameHierarchy, const ParseTypeUsage& underlyingType,
	AccessType access
){
	log("typedef", utility::join(nameHierarchy, "::") + " -> " + underlyingType.dataType.getFullTypeName(), location);

	Node* node = addNodeHierarchy(Node::NODE_TYPEDEF, nameHierarchy);
	addAccess(node, access);
	addTokenLocation(node, location);
	addTypeEdge(node, Edge::EDGE_TYPEDEF_OF, underlyingType);

	return node->getId();
}

Id Storage::onClassParsed(
	const ParseLocation& location, const std::vector<std::string>& nameHierarchy, AccessType access,
	const ParseLocation& scopeLocation
){
	log("class", utility::join(nameHierarchy, "::"), location);

	Node* node = addNodeHierarchy(Node::NODE_CLASS, nameHierarchy);
	addAccess(node, access);
	addTokenLocation(node, location);
	addTokenLocation(node, scopeLocation, true);

	return node->getId();
}

Id Storage::onStructParsed(
	const ParseLocation& location, const std::vector<std::string>& nameHierarchy, AccessType access,
	const ParseLocation& scopeLocation
){
	log("struct", utility::join(nameHierarchy, "::"), location);

	Node* node = addNodeHierarchy(Node::NODE_STRUCT, nameHierarchy);
	addAccess(node, access);
	addTokenLocation(node, location);
	addTokenLocation(node, scopeLocation, true);

	return node->getId();
}

Id Storage::onGlobalVariableParsed(const ParseLocation& location, const ParseVariable& variable)
{
	log("global", variable.getFullName(), location);

	Node* node = addNodeHierarchy(Node::NODE_GLOBAL_VARIABLE, variable.nameHierarchy);

	if (variable.isStatic)
	{
		node->addComponentStatic(std::make_shared<TokenComponentStatic>());
	}

	addTypeEdge(node, Edge::EDGE_TYPE_OF, variable.type);
	addTokenLocation(node, location);

	return node->getId();
}

Id Storage::onFieldParsed(const ParseLocation& location, const ParseVariable& variable, AccessType access)
{
	log("field", variable.getFullName(), location);

	Node* node = addNodeHierarchy(Node::NODE_FIELD, variable.nameHierarchy);

	if (!node->getMemberEdge())
	{
		LOG_ERROR("Field is not a member of anything.");
	}

	if (variable.isStatic)
	{
		node->addComponentStatic(std::make_shared<TokenComponentStatic>());
	}

	if (access == ACCESS_NONE)
	{
		LOG_ERROR("Field needs to have access type [public, protected, private] but has none.");
	}
	addAccess(node, access);

	addTypeEdge(node, Edge::EDGE_TYPE_OF, variable.type);
	addTokenLocation(node, location);

	return node->getId();
}

Id Storage::onFunctionParsed(
	const ParseLocation& location, const ParseFunction& function, const ParseLocation& scopeLocation
){
	log("function", function.getFullName(), location);

	Node* node = addNodeHierarchyWithDistinctSignature(Node::NODE_FUNCTION, function);

	addTokenLocation(node, location);
	addTokenLocation(node, scopeLocation, true);

	addTypeEdge(node, Edge::EDGE_RETURN_TYPE_OF, function.returnType);
	for (const ParseTypeUsage& parameter : function.parameters)
	{
		addTypeEdge(node, Edge::EDGE_PARAMETER_TYPE_OF, parameter);
	}

	return node->getId();
}

Id Storage::onMethodParsed(
	const ParseLocation& location, const ParseFunction& method, AccessType access, AbstractionType abstraction,
	const ParseLocation& scopeLocation
){
	log("method", method.getFullName(), location);

	Node* node = addNodeHierarchyWithDistinctSignature(Node::NODE_METHOD, method);

	if (!node->getMemberEdge())
	{
		LOG_ERROR("Method is not a member of anything.");
	}

	if (method.isConst)
	{
		node->addComponentConst(std::make_shared<TokenComponentConst>());
	}

	if (method.isStatic)
	{
		node->addComponentStatic(std::make_shared<TokenComponentStatic>());
	}

	if (access == ACCESS_NONE)
	{
		LOG_ERROR("Method needs to have access type [public, protected, private] but has none.");
	}
	addAccess(node, access);
	addAbstraction(node, abstraction);

	addTokenLocation(node, location);
	addTokenLocation(node, scopeLocation, true);

	addTypeEdge(node, Edge::EDGE_RETURN_TYPE_OF, method.returnType);
	for (const ParseTypeUsage& parameter : method.parameters)
	{
		addTypeEdge(node, Edge::EDGE_PARAMETER_TYPE_OF, parameter);
	}

	return node->getId();
}

Id Storage::onNamespaceParsed(
	const ParseLocation& location, const std::vector<std::string>& nameHierarchy, const ParseLocation& scopeLocation
){
	log("namespace", utility::join(nameHierarchy, "::"), location);

	Node* node = addNodeHierarchy(Node::NODE_NAMESPACE, nameHierarchy);
	addTokenLocation(node, location);
	addTokenLocation(node, scopeLocation, true);

	return node->getId();
}

Id Storage::onEnumParsed(
	const ParseLocation& location, const std::vector<std::string>& nameHierarchy, AccessType access,
	const ParseLocation& scopeLocation
){
	log("enum", utility::join(nameHierarchy, "::"), location);

	Node* node = addNodeHierarchy(Node::NODE_ENUM, nameHierarchy);
	addAccess(node, access);
	addTokenLocation(node, location);
	addTokenLocation(node, scopeLocation, true);

	return node->getId();
}

Id Storage::onEnumFieldParsed(const ParseLocation& location, const std::vector<std::string>& nameHierarchy)
{
	log("enum field", utility::join(nameHierarchy, "::"), location);

	Node* node = addNodeHierarchy(Node::NODE_FIELD, nameHierarchy);
	addTokenLocation(node, location);

	return node->getId();
}

Id Storage::onInheritanceParsed(
	const ParseLocation& location, const std::vector<std::string>& nameHierarchy,
	const std::vector<std::string>& baseNameHierarchy, AccessType access
){
	log("inheritance", utility::join(nameHierarchy, "::") + " : " + utility::join(baseNameHierarchy, "::"), location);

	Node* node = addNodeHierarchy(Node::NODE_UNDEFINED_TYPE, nameHierarchy);
	Node* baseNode = addNodeHierarchy(Node::NODE_UNDEFINED_TYPE, baseNameHierarchy);

	Edge* edge = m_graph.createEdge(Edge::EDGE_INHERITANCE, node, baseNode);
	edge->addComponentAccess(std::make_shared<TokenComponentAccess>(convertAccessType(access)));

	addTokenLocation(edge, location);

	return edge->getId();
}

Id Storage::onCallParsed(const ParseLocation& location, const ParseFunction& caller, const ParseFunction& callee)
{
	log("call", caller.getFullName() + " -> " + callee.getFullName(), location);

	Node* callerNode = addNodeHierarchyWithDistinctSignature(Node::NODE_UNDEFINED_FUNCTION, caller);
	Node* calleeNode = addNodeHierarchyWithDistinctSignature(Node::NODE_UNDEFINED_FUNCTION, callee);

	Edge* edge = m_graph.createEdge(Edge::EDGE_CALL, callerNode, calleeNode);

	addTokenLocation(edge, location);

	return edge->getId();
}

Id Storage::onCallParsed(const ParseLocation& location, const ParseVariable& caller, const ParseFunction& callee)
{
	log("call", caller.getFullName() + " -> " + callee.getFullName(), location);

	Node* callerNode = addNodeHierarchy(Node::NODE_UNDEFINED, caller.nameHierarchy);
	Node* calleeNode = addNodeHierarchyWithDistinctSignature(Node::NODE_UNDEFINED_FUNCTION, callee);

	Edge* edge = m_graph.createEdge(Edge::EDGE_CALL, callerNode, calleeNode);

	addTokenLocation(edge, location);

	return edge->getId();
}

Id Storage::onVariableUsageParsed(
	const std::string kind, const ParseLocation& location, const ParseFunction& user,
	const std::vector<std::string>& usedNameHierarchy
){
	log(kind, user.getFullName() + " -> " + utility::join(usedNameHierarchy, "::"), location);

	Node* userNode = addNodeHierarchyWithDistinctSignature(Node::NODE_UNDEFINED_FUNCTION, user);
	Node* usedNode = addNodeHierarchy(Node::NODE_UNDEFINED_VARIABLE, usedNameHierarchy);

	Edge* edge = m_graph.createEdge(Edge::EDGE_USAGE, userNode, usedNode);
	addTokenLocation(edge, location);

	return edge->getId();
}

Id Storage::onFieldUsageParsed(
	const ParseLocation& location, const ParseFunction& user, const std::vector<std::string>& usedNameHierarchy
){
	return onVariableUsageParsed("field usage", location, user, usedNameHierarchy);
}

Id Storage::onGlobalVariableUsageParsed( // or static variable used
	const ParseLocation& location, const ParseFunction& user, const std::vector<std::string>& usedNameHierarchy
){
	return onVariableUsageParsed("global usage", location, user, usedNameHierarchy);
}

Id Storage::onEnumFieldUsageParsed(
		const ParseLocation& location, const ParseFunction& user, const std::vector<std::string>& usedNameHierarchy
){
	return onVariableUsageParsed("enum field usage", location, user, usedNameHierarchy);
}

Id Storage::onEnumFieldUsageParsed(
		const ParseLocation& location, const ParseVariable& user, const std::vector<std::string>& usedNameHierarchy
){
	log("enum field usage", user.getFullName() + " -> " + utility::join(usedNameHierarchy, "::"), location);

	Node* userNode = addNodeHierarchy(Node::NODE_UNDEFINED_VARIABLE, user.nameHierarchy);
	Node* usedNode = addNodeHierarchy(Node::NODE_UNDEFINED_VARIABLE, usedNameHierarchy);

	Edge* edge = m_graph.createEdge(Edge::EDGE_USAGE, userNode, usedNode);
	addTokenLocation(edge, location);

	return edge->getId();
}

Id Storage::onTypeUsageParsed(const ParseTypeUsage& type, const ParseFunction& function)
{
	log("type usage", function.getFullName() + " -> " + type.dataType.getRawTypeName(), type.location);

	Node* functionNode = addNodeHierarchyWithDistinctSignature(Node::NODE_UNDEFINED_FUNCTION, function);
	Edge* edge = addTypeEdge(functionNode, Edge::EDGE_TYPE_USAGE, type);

	if (!edge)
	{
		LOG_ERROR("Could not create type usage edge.");
		return 0;
	}

	return edge->getId();
}

Id Storage::onTypeUsageParsed(const ParseTypeUsage& type, const ParseVariable& variable)
{
	log("type usage", variable.getFullName() + " -> " + type.dataType.getRawTypeName(), type.location);

	Node* variableNode = addNodeHierarchy(Node::NODE_UNDEFINED, variable.nameHierarchy);
	Edge* edge = addTypeEdge(variableNode, Edge::EDGE_TYPE_USAGE, type);

	if (!edge)
	{
		LOG_ERROR("Could not create type usage edge.");
		return 0;
	}

	return edge->getId();
}

Id Storage::onTemplateRecordParameterTypeParsed(
	const ParseLocation& location, const std::string& templateParameterTypeName,
	const std::vector<std::string>& templateRecordNameHierarchy
)
{
	log("template record type parameter", templateParameterTypeName, location);
	std::vector<std::string> templateParameterTypeNameHierarchy = templateRecordNameHierarchy;
	templateParameterTypeNameHierarchy.back() += "::" + templateParameterTypeName;
	Node* templateParameterNode = addNodeHierarchy(Node::NODE_TEMPLATE_PARAMETER_TYPE, templateParameterTypeNameHierarchy);
	addTokenLocation(templateParameterNode, location);

	Node* templateRecordNode = addNodeHierarchy(Node::NODE_UNDEFINED_TYPE, templateRecordNameHierarchy);
	Edge* edge = m_graph.createEdge(Edge::EDGE_TEMPLATE_PARAMETER_OF, templateParameterNode, templateRecordNode);

	return 0;
}

Id Storage::onTemplateRecordArgumentTypeParsed(
	const ParseLocation& location, const std::vector<std::string>& templateArgumentTypeNameHierarchy,
	const std::vector<std::string>& templateRecordNameHierarchy)
{
	log("template record argument", utility::join(templateArgumentTypeNameHierarchy, "::") + " -> " + utility::join(templateRecordNameHierarchy, "::"), location);
	Node* templateArgumentNode = addNodeHierarchy(Node::NODE_UNDEFINED_TYPE, templateArgumentTypeNameHierarchy);
	Node* templateRecordNode = addNodeHierarchy(Node::NODE_UNDEFINED_TYPE, templateRecordNameHierarchy);
	Edge* edge = m_graph.createEdge(Edge::EDGE_TEMPLATE_ARGUMENT_OF, templateArgumentNode, templateRecordNode);
	if (location.isValid())
	{
		addTokenLocation(templateArgumentNode, location);
	}
	return 0;
}

Id Storage::onTemplateRecordSpecializationParsed(
	const ParseLocation& location, const std::vector<std::string>& specializedRecordNameHierarchy,
		const RecordType specializedRecordType, const std::vector<std::string>& specializedFromNameHierarchy)
{
	log("template record specialization", utility::join(specializedRecordNameHierarchy, "::") + " -> " + utility::join(specializedFromNameHierarchy, "::"), location);

	Node::NodeType specializedRecordNodeType = Node::NODE_CLASS;
	if (specializedRecordType == ParserClient::RECORD_STRUCT)
	{
		specializedRecordNodeType = Node::NODE_STRUCT;
	}

	Node* specializedRecordNode = addNodeHierarchy(specializedRecordNodeType, specializedRecordNameHierarchy);
	Node* templateRecordNode = addNodeHierarchy(Node::NODE_UNDEFINED_TYPE, specializedFromNameHierarchy);

	Edge* edge = m_graph.createEdge(Edge::EDGE_TEMPLATE_SPECIALIZATION_OF, specializedRecordNode, templateRecordNode);
	//addTokenLocation(edge, location);

	return 0;
}

Id Storage::onTemplateFunctionParameterTypeParsed(
	const ParseLocation& location, const std::string& templateParameterTypeName, const ParseFunction function
)
{
	log("function template type parameter", templateParameterTypeName, location);

	std::vector<std::string> templateParameterTypeNameHierarchy;
	templateParameterTypeNameHierarchy.push_back(function.getFullName() + "::"+ templateParameterTypeName);
	Node* templateParameterNode = addNodeHierarchy(Node::NODE_TEMPLATE_PARAMETER_TYPE, templateParameterTypeNameHierarchy);
	addTokenLocation(templateParameterNode, location);

	Node* templateFunctionNode = addNodeHierarchyWithDistinctSignature(Node::NODE_UNDEFINED_FUNCTION, function);

	Edge* edge = m_graph.createEdge(Edge::EDGE_TEMPLATE_PARAMETER_OF, templateParameterNode, templateFunctionNode);

	return 0;
}

Id Storage::onTemplateFunctionSpecializationParsed(
	const ParseLocation& location, const ParseFunction specializedFunction, const ParseFunction templateFunction
)
{
	log("function template specialization", specializedFunction.getFullName(), location);

	Node* specializedFunctionNode = addNodeHierarchyWithDistinctSignature(Node::NODE_UNDEFINED_FUNCTION, specializedFunction);
	Node* templateFunctionNode = addNodeHierarchyWithDistinctSignature(Node::NODE_UNDEFINED_FUNCTION, templateFunction);

	Edge* edge = m_graph.createEdge(Edge::EDGE_TEMPLATE_SPECIALIZATION_OF, specializedFunctionNode, templateFunctionNode);

	return 0;
}

Id Storage::getIdForNodeWithName(const std::string& fullName) const
{
	SearchNode* node = m_tokenIndex.getNode(fullName);
	if (node)
	{
		return node->getFirstTokenId();
	}
	return 0;
}

std::string Storage::getNameForNodeWithId(Id id) const
{
	Token* token = m_graph.getTokenById(id);

	if (!token)
	{
		return "";
	}

	if (token->isEdge())
	{
		return dynamic_cast<Edge*>(token)->getName();
	}
	else
	{
		return dynamic_cast<Node*>(token)->getFullName();
	}
}

std::vector<SearchMatch> Storage::getAutocompletionMatches(
	const std::string& query, const std::string& word) const
{
	SearchResults tokenResults;

	bool usedSubquery = false;
	if (query.size())
	{
		usedSubquery = getSubQuerySearchResults(query, word, &tokenResults);
	}

	if (!usedSubquery && word.size())
	{
		tokenResults = m_tokenIndex.runFuzzySearch(word);
	}

	SearchResults filterResults = m_filterIndex.runFuzzySearch(word);
	tokenResults.insert(filterResults.begin(), filterResults.end());

	std::vector<SearchMatch> matches = SearchIndex::getMatches(tokenResults, word);
	SearchMatch::log(matches, word);

	for (SearchMatch& match : matches)
	{
		if (!match.tokenIds.size())
		{
			continue;
		}

		Token* token = m_graph.getTokenById(*match.tokenIds.cbegin());
		match.typeName = token->getTypeString();
	}

	return matches;
}

std::shared_ptr<Graph> Storage::getGraphForActiveTokenIds(const std::vector<Id>& tokenIds) const
{
	std::shared_ptr<Graph> graph = std::make_shared<Graph>();

	if (!tokenIds.size())
	{
		return graph;
	}

	if (tokenIds.size() > 1)
	{
		for (Id tokenId : tokenIds)
		{
			Token* token = m_graph.getTokenById(tokenId);
			if (!token)
			{
				LOG_ERROR_STREAM(<< "Token with id " << tokenId << " was not found");
				continue;
			}

			if (token->isNode())
			{
				Node* node = dynamic_cast<Node*>(token);
				graph->addNodeAndAllChildrenAsPlainCopy(node->getLastParentNode());
			}
			else
			{
				Edge* edge = dynamic_cast<Edge*>(token);
				graph->addEdgeAndAllChildrenAsPlainCopy(edge);
			}
		}
	}
	else if (tokenIds.size() == 1)
	{
		Token* token = m_graph.getTokenById(tokenIds[0]);

		if (!token)
		{
			LOG_ERROR_STREAM(<< "Token with id " << tokenIds[0] << " was not found");
			return graph;
		}

		if (token->isNode())
		{
			Node* node = dynamic_cast<Node*>(token);
			graph->addNodeAndAllChildrenAsPlainCopy(node->getLastParentNode());

			node->forEachEdge(
				[graph, node](Edge* edge)
				{
					const Node::NodeTypeMask varFuncMask =
						Node::NODE_UNDEFINED_FUNCTION | Node::NODE_FUNCTION | Node::NODE_METHOD |
						Node::NODE_UNDEFINED_VARIABLE | Node::NODE_GLOBAL_VARIABLE | Node::NODE_FIELD;

					if (!node->isType(varFuncMask) || !edge->isType(Edge::EDGE_AGGREGATION))
					{
						graph->addEdgeAndAllChildrenAsPlainCopy(edge);
					}
				}
			);
		}
		else
		{
			Edge* edge = dynamic_cast<Edge*>(token);
			graph->addEdgeAndAllChildrenAsPlainCopy(edge);
		}
	}

	for(const std::pair<Id, std::shared_ptr<Node>> nodePair : graph->getNodes())
	{
		Node* node = m_graph.getNodeById(nodePair.first);

		node->forEachEdge(
			[graph](Edge* edge)
			{
				if(edge->getType() != Edge::EdgeType::EDGE_MEMBER)
				{
					Node* from = edge->getFrom();
					Node* to = edge->getTo();

					if(graph->findNode([from](Node* node){return from->getId() == node->getId();}) != NULL
						&& graph->findNode([to](Node* node){return to->getId() == node->getId();}) != NULL)
					{
						graph->addEdge(edge);
					}
				}
			}
		);
	}

	return graph;
}

std::vector<Id> Storage::getActiveTokenIdsForId(Id tokenId, Id* declarationId) const
{
	std::vector<Id> ret;

	Token* token = m_graph.getTokenById(tokenId);
	if (!token)
	{
		return ret;
	}

	ret.push_back(token->getId());

	if (token->isNode())
	{
		Node* node = dynamic_cast<Node*>(token);
		*declarationId = node->getId();

		node->forEachEdge(
			[&node, &ret](Edge* edge)
			{
				if (edge->getTo() == node)
				{
					ret.push_back(edge->getId());
				}
			}
		);
	}

	return ret;
}

std::vector<Id> Storage::getActiveTokenIdsForLocationId(Id locationId) const
{
	std::vector<Id> ret;

	TokenLocation* location = m_locationCollection.findTokenLocationById(locationId);
	if (!location)
	{
		return ret;
	}

	Token* token = m_graph.getTokenById(location->getTokenId());
	if (!token)
	{
		return ret;
	}

	if (token->isNode())
	{
		Node* node = dynamic_cast<Node*>(token);
		ret.push_back(node->getId());
	}
	else
	{
		Edge* edge = dynamic_cast<Edge*>(token);
		ret.push_back(edge->getTo()->getId());
	}

	return ret;
}

std::vector<Id> Storage::getLocationIdsForTokenIds(const std::vector<Id>& tokenIds) const
{
	std::vector<Id> ret;

	for (Id tokenId : tokenIds)
	{
		Token* token = m_graph.getTokenById(tokenId);
		if (!token)
		{
			continue;
		}

		ret.insert(ret.end(), token->getLocationIds().begin(), token->getLocationIds().end());
	}

	return ret;
}

std::vector<Id> Storage::getTokenIdsForQuery(std::string query) const
{
	QueryTree tree(query);
	GraphFilterConductor conductor;
	SubGraph outGraph;

	conductor.filter(&tree, &m_graph, &outGraph);

	LOG_INFO_STREAM(<< '\n' << tree << '\n' << outGraph);

	return outGraph.getTokenIds();
}

TokenLocationCollection Storage::getTokenLocationsForLocationIds(const std::vector<Id>& locationIds) const
{
	TokenLocationCollection ret;

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

TokenLocationFile Storage::getTokenLocationsForFile(const std::string& filePath) const
{
	TokenLocationFile ret(filePath);

	TokenLocationFile* locationFile = m_locationCollection.findTokenLocationFileByPath(filePath);
	if (!locationFile)
	{
		return ret;
	}

	locationFile->forEachTokenLocation(
		[&](TokenLocation* tokenLocation) -> void
		{
			ret.addTokenLocationAsPlainCopy(tokenLocation);
		}
	);

	return ret;
}

TokenLocationFile Storage::getTokenLocationsForLinesInFile(
		const std::string& filePath, uint firstLineNumber, uint lastLineNumber
) const
{
	TokenLocationFile ret(filePath);

	TokenLocationFile* locationFile = m_locationCollection.findTokenLocationFileByPath(filePath);
	if (!locationFile)
	{
		return ret;
	}

	uint endLineNumber = locationFile->getTokenLocationLines().rbegin()->first;

	for (uint i = firstLineNumber; i <= endLineNumber; i++)
	{
		TokenLocationLine* locationLine = locationFile->findTokenLocationLineByNumber(i);
		if (!locationLine)
		{
			continue;
		}

		if (locationLine->getLineNumber() <= lastLineNumber)
		{
			locationLine->forEachTokenLocation(
				[&](TokenLocation* tokenLocation) -> void
				{
					ret.addTokenLocationAsPlainCopy(tokenLocation);
				}
			);
		}
		else
		{
			// Save start locations of TokenLocations that span accross the line range.
			locationLine->forEachTokenLocation(
				[&](TokenLocation* tokenLocation) -> void
				{
					if (tokenLocation->isEndTokenLocation() &&
						tokenLocation->getStartTokenLocation()->getLineNumber() < firstLineNumber)
					{
						ret.addTokenLocationAsPlainCopy(tokenLocation->getStartTokenLocation());
					}
				}
			);
		}
	}

	return ret;
}

TokenLocationCollection Storage::getErrorTokenLocations(std::vector<std::string>* errorMessages) const
{
	errorMessages->insert(errorMessages->begin(), m_errorMessages.begin(), m_errorMessages.end());

	return m_errorLocationCollection;
}

const Graph& Storage::getGraph() const
{
	return m_graph;
}

const TokenLocationCollection& Storage::getTokenLocationCollection() const
{
	return m_locationCollection;
}

const SearchIndex& Storage::getSearchIndex() const
{
	return m_tokenIndex;
}

Node* Storage::addNodeHierarchy(Node::NodeType type, std::vector<std::string> nameHierarchy)
{
	SearchNode* searchNode = m_tokenIndex.addNode(nameHierarchy);
	if (!searchNode)
	{
		LOG_ERROR("No SearchNode");
		return nullptr;
	}

	return m_graph.createNodeHierarchy(type, searchNode);
}

Node* Storage::addNodeHierarchyWithDistinctSignature(Node::NodeType type, const ParseFunction& function)
{
	SearchNode* searchNode = m_tokenIndex.addNode(function.nameHierarchy);
	if (!searchNode)
	{
		LOG_ERROR("No SearchNode");
		return nullptr;
	}

	// TODO: Instead of saving the whole signature string, the signature should be just a set of wordIds.
	Id signatureId = m_tokenIndex.getWordId(ParserClient::functionSignatureStr(function));
	std::shared_ptr<TokenComponentSignature> signature = std::make_shared<TokenComponentSignature>(signatureId);

	return m_graph.createNodeHierarchyWithDistinctSignature(type, searchNode, signature);
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
	if (access == ACCESS_NONE)
	{
		return nullptr;
	}

	Edge* edge = node->getMemberEdge();
	if (!edge)
	{
		LOG_ERROR_STREAM(<< "Cannot assign access" << access << " to node " << node->getFullName() << " because it is not a child.");
		return nullptr;
	}

	std::shared_ptr<TokenComponentAccess> ptr = std::make_shared<TokenComponentAccess>(convertAccessType(access));
	edge->addComponentAccess(ptr);
	return ptr.get();
}

TokenComponentAbstraction::AbstractionType Storage::convertAbstractionType(ParserClient::AbstractionType abstraction) const
{
	switch (abstraction)
	{
	case ABSTRACTION_VIRTUAL:
		return TokenComponentAbstraction::ABSTRACTION_VIRTUAL;
	case ABSTRACTION_PURE_VIRTUAL:
		return TokenComponentAbstraction::ABSTRACTION_PURE_VIRTUAL;
	case ABSTRACTION_NONE:
		return TokenComponentAbstraction::ABSTRACTION_NONE;
	}
}

TokenComponentAbstraction* Storage::addAbstraction(Node* node, ParserClient::AbstractionType abstraction)
{
	if (abstraction != ABSTRACTION_NONE)
	{
		std::shared_ptr<TokenComponentAbstraction> ptr =
			std::make_shared<TokenComponentAbstraction>(convertAbstractionType(abstraction));
		node->addComponentAbstraction(ptr);
		return ptr.get();
	}
	return nullptr;
}

Edge* Storage::addTypeEdge(Node* node, Edge::EdgeType edgeType, const ParseTypeUsage& typeUsage)
{
	if (!typeUsage.location.isValid())
	{
		return nullptr;
	}

	std::vector<std::string> nameHierarchy = typeUsage.dataType.getTypeNameHierarchy();

	Node* typeNode = addNodeHierarchy(Node::NODE_UNDEFINED_TYPE, nameHierarchy);
	if (!typeNode)
	{
		return nullptr;
	}

	Edge* edge = m_graph.createEdge(edgeType, node, typeNode);
	addTokenLocation(edge, typeUsage.location);

	return edge;
}

TokenLocation* Storage::addTokenLocation(Token* token, const ParseLocation& loc, bool isScope)
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

	if (isScope)
	{
		location->setType(TokenLocation::LOCATION_SCOPE);
	}

	token->addLocationId(location->getId());
	return location;
}

bool Storage::getSubQuerySearchResults(
	const std::string& query,
	const std::string& word,
	SearchResults* results
) const {
	std::string q = query;

	if (QueryOperator::getOperatorType(q.back()) == QueryOperator::OPERATOR_SUB)
	{
		q.pop_back();
	}
	else if (QueryOperator::getOperatorType(q.back()) == QueryOperator::OPERATOR_HAS)
	{
		q.pop_back();
	}
	else if (QueryOperator::getOperatorType(q.back()) != QueryOperator::OPERATOR_NONE)
	{
		return false;
	}

	QueryTree tree(q);
	if (!tree.isValid())
	{
		return false;
	}

	SubGraph graph;
	GraphFilterConductor conductor;
	conductor.filter(&tree, &m_graph, &graph);

	std::vector<const SearchNode*> searchNodes;
	graph.forEachNode(
		[&searchNodes](Node* node)
		{
			const TokenComponentName* nameComponent = node->getTokenComponentName();
			if (nameComponent)
			{
				searchNodes.push_back(nameComponent->getSearchNode());
			}
		}
	);

	for (const SearchNode* node : searchNodes)
	{
		if (word.size())
		{
			if (searchNodes.size() > 1)
			{
				SearchResults res = node->runFuzzySearchOnSelf(word);
				results->insert(res.begin(), res.end());
			}
			else
			{
				SearchResults res = node->runFuzzySearch(word);
				results->insert(res.begin(), res.end());
			}
		}
		else if (searchNodes.size() == 1)
		{
			for (const std::shared_ptr<SearchNode>& child : node->getChildren())
			{
				child->addResultsRecursive(*results, 0, child.get());
			}
		}
		else
		{
			node->addResultsRecursive(*results, 0, node);
		}
	}

	return true;
}

void Storage::removeNodeIfUnreferenced(Node* node)
{
	Id tokenId = node->getId();
	SearchNode* searchNode = m_tokenIndex.getNode(node->getTokenComponentName()->getSearchNode());

	bool removed = m_graph.removeNodeIfUnreferencedRecursive(node);

	if (removed && searchNode)
	{
		searchNode->removeTokenId(tokenId);
		m_tokenIndex.removeNodeIfUnreferencedRecursive(searchNode);
	}
}

void Storage::log(std::string type, std::string str, const ParseLocation& location) const
{
	LOG_INFO_STREAM(
		<< type << ": " << str << " <" << location.filePath << " "
		<< location.startLineNumber << ":" << location.startColumnNumber << " "
		<< location.endLineNumber << ":" << location.endColumnNumber << ">"
	);
}
