#include "data/Storage.h"

#include "utility/logging/logging.h"
#include "utility/utilityString.h"
#include "utility/file/FileSystem.h"

#include "data/graph/filter/GraphFilterConductor.h"
#include "data/graph/token_component/TokenComponentConst.h"
#include "data/graph/token_component/TokenComponentName.h"
#include "data/graph/token_component/TokenComponentStatic.h"
#include "data/graph/token_component/TokenComponentFilePath.h"
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
#include "settings/ApplicationSettings.h"

Storage::Storage()
{
	for (const std::pair<std::string, QueryCommand::CommandType>& p : QueryCommand::getCommandTypeMap())
	{
		NameHierarchy commandNameHierarchy;
		commandNameHierarchy.push(std::make_shared<NameElement>(p.first));
		m_filterIndex.addNode(commandNameHierarchy);
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

void Storage::clearFileData(const std::set<FilePath>& filePaths)
{
	for (const FilePath& filePath : filePaths)
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

std::set<FilePath> Storage::getDependingFilePathsAndRemoveFileNodes(const std::set<FilePath>& filePaths)
{
	std::set<FilePath> dependingFilePaths;

	for (const FilePath& filePath : filePaths)
	{
		Node* fileNode = findFileNode(filePath);

		if (!fileNode->getComponent<TokenComponentFilePath>() ||
			fileNode->getComponent<TokenComponentFilePath>()->getFilePath() != filePath)
		{
			LOG_ERROR("Node is not resolving to the same file.");
			continue;
		}

		addDependingFilePathsAndRemoveFileNodesRecursive(fileNode, &dependingFilePaths);
	}

	for (const FilePath& path : filePaths)
	{
		dependingFilePaths.erase(path);
	}

	return dependingFilePaths;
}

void Storage::logGraph() const
{
	LOG_INFO_STREAM(<< '\n' << m_graph);
}

void Storage::logLocations() const
{
	LOG_INFO_STREAM(<< '\n' << m_locationCollection);
}

void Storage::logIndex() const
{
	LOG_INFO_STREAM(<< '\n' << m_tokenIndex);
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

		m_errorLocationCollection.addTokenLocation(
			errorId, filePath,
			location.startLineNumber, location.startColumnNumber,
			location.endLineNumber, location.endColumnNumber
		);

		m_errorMessages.push_back(message);
	}
}

size_t Storage::getErrorCount() const
{
	return m_errorLocationCollection.getTokenLocationCount();
}

Id Storage::onTypedefParsed(
	const ParseLocation& location, const NameHierarchy& nameHierarchy, const ParseTypeUsage& underlyingType,
	AccessType access
){
	log("typedef", nameHierarchy.getFullName() + " -> " + underlyingType.dataType->getFullTypeName(), location);

	Node* node = addNodeHierarchy(Node::NODE_TYPEDEF, nameHierarchy);
	addAccess(node, access);
	addTokenLocation(node, location);
	addTypeEdge(node, Edge::EDGE_TYPEDEF_OF, underlyingType);

	return node->getId();
}

Id Storage::onClassParsed(
	const ParseLocation& location, const NameHierarchy& nameHierarchy, AccessType access,
	const ParseLocation& scopeLocation
){
	log("class", nameHierarchy.getFullName(), location);

	Node* node = addNodeHierarchy(Node::NODE_CLASS, nameHierarchy);
	addAccess(node, access);
	addTokenLocation(node, location);
	addTokenLocation(node, scopeLocation, true);

	return node->getId();
}

Id Storage::onStructParsed(
	const ParseLocation& location, const NameHierarchy& nameHierarchy, AccessType access,
	const ParseLocation& scopeLocation
){
	log("struct", nameHierarchy.getFullName(), location);

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
	const ParseLocation& location, const NameHierarchy& nameHierarchy, const ParseLocation& scopeLocation
){
	log("namespace", nameHierarchy.getFullName(), location);

	Node* node = addNodeHierarchy(Node::NODE_NAMESPACE, nameHierarchy);
	addTokenLocation(node, location);
	addTokenLocation(node, scopeLocation, true);

	return node->getId();
}

Id Storage::onEnumParsed(
	const ParseLocation& location, const NameHierarchy& nameHierarchy, AccessType access,
	const ParseLocation& scopeLocation
){
	log("enum", nameHierarchy.getFullName(), location);

	Node* node = addNodeHierarchy(Node::NODE_ENUM, nameHierarchy);
	addAccess(node, access);
	addTokenLocation(node, location);
	addTokenLocation(node, scopeLocation, true);

	return node->getId();
}

Id Storage::onEnumConstantParsed(const ParseLocation& location, const NameHierarchy& nameHierarchy)
{
	log("enum constant", nameHierarchy.getFullName(), location);

	Node* node = addNodeHierarchy(Node::NODE_ENUM_CONSTANT, nameHierarchy);
	addTokenLocation(node, location);

	return node->getId();
}

Id Storage::onInheritanceParsed(
	const ParseLocation& location, const NameHierarchy& nameHierarchy,
	const NameHierarchy& baseNameHierarchy, AccessType access
){
	log("inheritance", nameHierarchy.getFullName() + " : " + baseNameHierarchy.getFullName(), location);

	Node* node = addNodeHierarchy(Node::NODE_UNDEFINED_TYPE, nameHierarchy);
	Node* baseNode = addNodeHierarchy(Node::NODE_UNDEFINED_TYPE, baseNameHierarchy);

	Edge* edge = m_graph.createEdge(Edge::EDGE_INHERITANCE, node, baseNode);
	edge->addComponentAccess(std::make_shared<TokenComponentAccess>(convertAccessType(access)));

	addTokenLocation(edge, location);

	return edge->getId();
}

Id Storage::onMethodOverrideParsed(
	const ParseLocation& location, const ParseFunction& base, const ParseFunction& overrider)
{
	log("override", base.getFullName() + " -> " + overrider.getFullName(), location);

	Node* baseNode = addNodeHierarchyWithDistinctSignature(Node::NODE_UNDEFINED_FUNCTION, base);
	Node* overriderNode = addNodeHierarchyWithDistinctSignature(Node::NODE_UNDEFINED_FUNCTION, overrider);

	Edge* edge = m_graph.createEdge(Edge::EDGE_OVERRIDE, baseNode, overriderNode);

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
	const NameHierarchy& usedNameHierarchy
){
	log(kind, user.getFullName() + " -> " + usedNameHierarchy.getFullName(), location);

	Node* userNode = addNodeHierarchyWithDistinctSignature(Node::NODE_UNDEFINED_FUNCTION, user);
	Node* usedNode = addNodeHierarchy(Node::NODE_UNDEFINED_VARIABLE, usedNameHierarchy);

	Edge* edge = m_graph.createEdge(Edge::EDGE_USAGE, userNode, usedNode);
	addTokenLocation(edge, location);

	return edge->getId();
}

Id Storage::onFieldUsageParsed(
	const ParseLocation& location, const ParseFunction& user, const NameHierarchy& usedNameHierarchy
){
	return onVariableUsageParsed("field usage", location, user, usedNameHierarchy);
}

Id Storage::onGlobalVariableUsageParsed( // or static variable used
	const ParseLocation& location, const ParseFunction& user, const NameHierarchy& usedNameHierarchy
){
	return onVariableUsageParsed("global usage", location, user, usedNameHierarchy);
}

Id Storage::onGlobalVariableUsageParsed(
	const ParseLocation& location, const ParseVariable& user, const NameHierarchy& usedNameHierarchy)
{
	log("global usage", user.getFullName() + " -> " + usedNameHierarchy.getFullName(), location);

	Node* userNode = addNodeHierarchy(Node::NODE_UNDEFINED_VARIABLE, user.nameHierarchy);
	Node* usedNode = addNodeHierarchy(Node::NODE_UNDEFINED_VARIABLE, usedNameHierarchy);

	Edge* edge = m_graph.createEdge(Edge::EDGE_USAGE, userNode, usedNode);
	addTokenLocation(edge, location);

	return edge->getId();
}

Id Storage::onEnumConstantUsageParsed(
		const ParseLocation& location, const ParseFunction& user, const NameHierarchy& usedNameHierarchy
){
	return onVariableUsageParsed("enum constant usage", location, user, usedNameHierarchy);
}

Id Storage::onEnumConstantUsageParsed(
		const ParseLocation& location, const ParseVariable& user, const NameHierarchy& usedNameHierarchy
){
	log("enum constant usage", user.getFullName() + " -> " + usedNameHierarchy.getFullName(), location);

	Node* userNode = addNodeHierarchy(Node::NODE_UNDEFINED_VARIABLE, user.nameHierarchy);
	Node* usedNode = addNodeHierarchy(Node::NODE_UNDEFINED_VARIABLE, usedNameHierarchy);

	Edge* edge = m_graph.createEdge(Edge::EDGE_USAGE, userNode, usedNode);
	addTokenLocation(edge, location);

	return edge->getId();
}

Id Storage::onTypeUsageParsed(const ParseTypeUsage& type, const ParseFunction& function)
{
	log("type usage", function.getFullName() + " -> " + type.dataType->getRawTypeName(), type.location);

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
	log("type usage", variable.getFullName() + " -> " + type.dataType->getRawTypeName(), type.location);

	Node* variableNode = addNodeHierarchy(Node::NODE_UNDEFINED, variable.nameHierarchy);
	Edge* edge = addTypeEdge(variableNode, Edge::EDGE_TYPE_USAGE, type);

	if (!edge)
	{
		LOG_ERROR("Could not create type usage edge.");
		return 0;
	}

	return edge->getId();
}

Id Storage::onTemplateArgumentTypeParsed(
		const ParseLocation& location, const NameHierarchy& argumentNameHierarchy,
		const NameHierarchy& templateNameHierarchy)
{
	log(
		"template argument type",
		argumentNameHierarchy.getFullName() + " -> " + templateNameHierarchy.getFullName(),
		location
	);

	Node* argumentNode = addNodeHierarchy(Node::NODE_UNDEFINED_TYPE, argumentNameHierarchy);
	Node* templateNode = addNodeHierarchy(Node::NODE_UNDEFINED_TYPE, templateNameHierarchy);
	Edge* argumentOfEdge = m_graph.createEdge(Edge::EDGE_TEMPLATE_ARGUMENT_OF, argumentNode, templateNode);

	addTokenLocation(argumentNode, location);
	addTokenLocation(argumentOfEdge, location);

	return argumentNode->getId();
}

Id Storage::onTemplateDefaultArgumentTypeParsed(
	const ParseTypeUsage& defaultArgumentType, const NameHierarchy& templateArgumentTypeNameHierarchy
){
	log(
		"template default argument",
		defaultArgumentType.dataType->getTypeNameHierarchy().getFullName() + " -> " + templateArgumentTypeNameHierarchy.getFullName(),
		defaultArgumentType.location
	);

	Node* templateDefaultArgumentNode =
		addNodeHierarchy(Node::NODE_UNDEFINED_TYPE, defaultArgumentType.dataType->getTypeNameHierarchy());
	Node* templateArgumentNode = addNodeHierarchy(Node::NODE_UNDEFINED_TYPE, templateArgumentTypeNameHierarchy);
	Edge* templateArgumentEdge =
		m_graph.createEdge(Edge::EDGE_TEMPLATE_DEFAULT_ARGUMENT_OF, templateDefaultArgumentNode, templateArgumentNode);

	addTokenLocation(templateDefaultArgumentNode, defaultArgumentType.location);
	addTokenLocation(templateArgumentEdge, defaultArgumentType.location);

	return templateDefaultArgumentNode->getId();
}

Id Storage::onTemplateRecordParameterTypeParsed(
	const ParseLocation& location, const NameHierarchy& templateParameterTypeNameHierarchy,
	const NameHierarchy& templateRecordNameHierarchy
){
	log("template record type parameter", templateParameterTypeNameHierarchy.getFullName(), location);

	Node* templateParameterNode = addNodeHierarchy(Node::NODE_TEMPLATE_PARAMETER_TYPE, templateParameterTypeNameHierarchy);
	Node* templateRecordNode = addNodeHierarchy(Node::NODE_UNDEFINED_TYPE, templateRecordNameHierarchy);
	Edge* templateParameterEdge =
		m_graph.createEdge(Edge::EDGE_TEMPLATE_PARAMETER_OF, templateParameterNode, templateRecordNode);

	addTokenLocation(templateParameterNode, location);
	addTokenLocation(templateParameterEdge, location);

	return templateParameterNode->getId();
}

Id Storage::onTemplateRecordSpecializationParsed(
	const ParseLocation& location, const NameHierarchy& specializedRecordNameHierarchy,
	const RecordType specializedRecordType, const NameHierarchy& specializedFromNameHierarchy
){
	log(
		"template record specialization",
		specializedRecordNameHierarchy.getFullName() + " -> " + specializedFromNameHierarchy.getFullName(),
		location
	);

	Node::NodeType specializedRecordNodeType = Node::NODE_CLASS;
	if (specializedRecordType == ParserClient::RECORD_STRUCT)
	{
		specializedRecordNodeType = Node::NODE_STRUCT;
	}

	Node* specializedRecordNode = addNodeHierarchy(specializedRecordNodeType, specializedRecordNameHierarchy);
	Node* templateRecordNode = addNodeHierarchy(Node::NODE_UNDEFINED_TYPE, specializedFromNameHierarchy);
	Edge* templateSpecializationEdge =
		m_graph.createEdge(Edge::EDGE_TEMPLATE_SPECIALIZATION_OF, specializedRecordNode, templateRecordNode);

	addTokenLocation(specializedRecordNode, location);
	addTokenLocation(templateSpecializationEdge, location);

	return specializedRecordNode->getId();
}

Id Storage::onTemplateFunctionParameterTypeParsed(
	const ParseLocation& location, const NameHierarchy& templateParameterTypeNameHierarchy, const ParseFunction function
){
	log("template function type parameter", templateParameterTypeNameHierarchy.getFullName(), location);

	Node* templateParameterNode = addNodeHierarchy(Node::NODE_TEMPLATE_PARAMETER_TYPE, templateParameterTypeNameHierarchy);
	Node* templateFunctionNode = addNodeHierarchyWithDistinctSignature(Node::NODE_UNDEFINED_FUNCTION, function);
	Edge* templateParameterEdge =
		m_graph.createEdge(Edge::EDGE_TEMPLATE_PARAMETER_OF, templateParameterNode, templateFunctionNode);

	addTokenLocation(templateParameterNode, location);
	addTokenLocation(templateParameterEdge, location);

	return templateParameterNode->getId();
}

Id Storage::onTemplateFunctionSpecializationParsed(
	const ParseLocation& location, const ParseFunction specializedFunction, const ParseFunction templateFunction
){
	log("function template specialization", specializedFunction.getFullName(), location);

	Node* specializedFunctionNode = addNodeHierarchyWithDistinctSignature(Node::NODE_UNDEFINED_FUNCTION, specializedFunction);
	Node* templateFunctionNode = addNodeHierarchyWithDistinctSignature(Node::NODE_UNDEFINED_FUNCTION, templateFunction);

	Edge* templateSpecializationEdge =
		m_graph.createEdge(Edge::EDGE_TEMPLATE_SPECIALIZATION_OF, specializedFunctionNode, templateFunctionNode);

	addTokenLocation(specializedFunctionNode, location);
	addTokenLocation(templateSpecializationEdge, location);

	return specializedFunctionNode->getId();
}

Id Storage::onFileParsed(const std::string& filePath)
{
	log("file", filePath, ParseLocation());

	Node* fileNode = addFileNode(filePath);
	return fileNode->getId();
}

Id Storage::onFileIncludeParsed(const ParseLocation& location, const std::string& filePath, const std::string& includedPath)
{
	log("include", includedPath, location);

	Node* fileNode = addFileNode(filePath);
	Node* includedFileNode = addFileNode(includedPath);

	Edge* edge = m_graph.createEdge(Edge::EDGE_INCLUDE, fileNode, includedFileNode);
	addTokenLocation(edge, location);

	return edge->getId();
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

Node::NodeType Storage::getNodeTypeForNodeWithId(Id id) const
{
	Token* token = m_graph.getTokenById(id);
	if(!token)
	{
		return Node::NODE_UNDEFINED;
	}
	if(!token->isEdge())
	{
		return dynamic_cast<Node*>(token)->getType();
	}
	return Node::NODE_UNDEFINED;
}

std::vector<SearchMatch> Storage::getAutocompletionMatches(const std::string& query, const std::string& word) const
{
	SearchResults tokenResults;

	bool hasQueryResults = false;
	if (query.size())
	{
		hasQueryResults = getQuerySearchResults(query, word, &tokenResults);
	}

	if (!hasQueryResults && word.size())
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
			match.queryNodeType = QueryNode::QUERYNODETYPE_COMMAND;
			continue;
		}

		Token* token = m_graph.getTokenById(*match.tokenIds.cbegin());
		if (token->isNode())
		{
			match.nodeType = dynamic_cast<Node*>(token)->getType();
		}
		match.typeName = token->getTypeString();
		match.queryNodeType = QueryNode::QUERYNODETYPE_TOKEN;
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

		for (const std::pair<Id, std::shared_ptr<Node>> nodePair : graph->getNodes())
		{
			Node* node = m_graph.getNodeById(nodePair.first);

			node->forEachEdge(
				[graph](Edge* edge)
				{
					if (edge->getType() != Edge::EdgeType::EDGE_MEMBER)
					{
						Node* from = edge->getFrom();
						Node* to = edge->getTo();

						if (graph->findNode([from](Node* node){ return from->getId() == node->getId(); }) != NULL &&
							graph->findNode([to](Node* node){ return to->getId() == node->getId(); }) != NULL)
						{
							graph->addEdge(edge);
						}
					}
				}
			);
		}
	}

	if (ApplicationSettings::getInstance()->filterUndefinedNodesFromGraph())
	{
		bool allUndefined = true;
		Node::NodeTypeMask undefinedMask =
			Node::NODE_UNDEFINED | Node::NODE_UNDEFINED_TYPE |
			Node::NODE_UNDEFINED_VARIABLE | Node::NODE_UNDEFINED_FUNCTION;
		for (Id tokenId : tokenIds)
		{
			Token* token = m_graph.getTokenById(tokenId);
			if (token && token->isNode() && !dynamic_cast<Node*>(token)->isType(undefinedMask))
			{
				allUndefined = false;
				break;
			}
		}

		if (!allUndefined)
		{
			QueryTree tree("!'undefined'");
			std::shared_ptr<Graph> outGraph = std::make_shared<Graph>();

			GraphFilterConductor().filter(&tree, graph.get(), outGraph.get());
			return outGraph;
		}
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

std::vector<Id> Storage::getTokenIdsForQuery(std::string query) const
{
	QueryTree tree(query);
	GraphFilterConductor conductor;
	SubGraph outGraph;

	conductor.filter(&tree, &m_graph, &outGraph);

	LOG_INFO_STREAM(<< '\n' << tree << '\n' << outGraph);

	return outGraph.getTokenIds();
}

Id Storage::getTokenIdForFileNode(const FilePath& filePath) const
{
	Node* fileNode = findFileNode(filePath);
	if (fileNode)
	{
		return fileNode->getId();
	}

	return 0;
}

TokenLocationCollection Storage::getTokenLocationsForTokenIds(const std::vector<Id>& tokenIds) const
{
	TokenLocationCollection ret;

	for (Id tokenId : tokenIds)
	{
		Token* token = m_graph.getTokenById(tokenId);
		if (!token)
		{
			continue;
		}

		if (token->isNode() && dynamic_cast<Node*>(token)->isType(Node::NODE_FILE))
		{
			ret.addTokenLocationFileAsPlainCopy(m_locationCollection.findTokenLocationFileByPath(
				dynamic_cast<Node*>(token)->getComponent<TokenComponentFilePath>()->getFilePath()));
		}
		else
		{
			for (Id locationId: token->getLocationIds())
			{
				TokenLocation* location = m_locationCollection.findTokenLocationById(locationId);
				if (location->getOtherTokenLocation())
				{
					ret.addTokenLocationAsPlainCopy(location);
					ret.addTokenLocationAsPlainCopy(location->getOtherTokenLocation());
				}
			}
		}
	}

	return ret;
}

std::shared_ptr<TokenLocationFile> Storage::getTokenLocationsForFile(const std::string& filePath) const
{
	std::shared_ptr<TokenLocationFile> ret = std::make_shared<TokenLocationFile>(filePath);

	TokenLocationFile* locationFile = m_locationCollection.findTokenLocationFileByPath(filePath);
	if (!locationFile)
	{
		return ret;
	}

	locationFile->forEachTokenLocation(
		[&](TokenLocation* tokenLocation) -> void
		{
			ret->addTokenLocationAsPlainCopy(tokenLocation);
		}
	);
	ret->isWholeCopy = true;

	return ret;
}

std::shared_ptr<TokenLocationFile> Storage::getTokenLocationsForLinesInFile(
		const std::string& filePath, uint firstLineNumber, uint lastLineNumber
) const
{
	std::shared_ptr<TokenLocationFile> ret = std::make_shared<TokenLocationFile>(filePath);

	TokenLocationFile* locationFile = m_locationCollection.findTokenLocationFileByPath(filePath);
	if (!locationFile)
	{
		return ret;
	}

	uint endLineNumber = locationFile->getTokenLocationLines().rbegin()->first;
	std::set<int> addedLocationIds;
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
					const Id tokenId = tokenLocation->getId();
					if (addedLocationIds.find(tokenId) == addedLocationIds.end())
					{
						ret->addTokenLocationAsPlainCopy(tokenLocation->getStartTokenLocation());
						ret->addTokenLocationAsPlainCopy(tokenLocation->getEndTokenLocation());
						addedLocationIds.insert(tokenId);
					}
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
						ret->addTokenLocationAsPlainCopy(tokenLocation->getStartTokenLocation());
						ret->addTokenLocationAsPlainCopy(tokenLocation->getEndTokenLocation());
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

std::shared_ptr<TokenLocationFile> Storage::getTokenLocationOfParentScope(const TokenLocation* child) const
{
	const TokenLocation* parent = child;
	const FilePath filePath = child->getFilePath();
	const TokenLocationFile* locationFile = m_locationCollection.findTokenLocationFileByPath(child->getFilePath());
	locationFile->forEachStartTokenLocation(
		[&](TokenLocation* tokenLocation) -> void
		{
			if (tokenLocation->getType() == TokenLocation::LOCATION_SCOPE &&
				(*tokenLocation) < *(child->getStartTokenLocation()) &&
				(*tokenLocation->getEndTokenLocation()) > *(child->getEndTokenLocation()))
			{
				if (parent == child)
				{
					parent = tokenLocation;
				}
				// since tokenLocation is a start location the > location indicates the scope that is closer to the child.
				else if ((*tokenLocation) > *parent)
				{
					parent = tokenLocation;
				}
			}
		}
	);

	std::shared_ptr<TokenLocationFile> file = std::make_shared<TokenLocationFile>(filePath);
	if (parent != child)
	{
		file->addTokenLocationAsPlainCopy(parent);
		file->addTokenLocationAsPlainCopy(parent->getOtherTokenLocation());
	}
	return file;
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

Node* Storage::addNodeHierarchy(Node::NodeType type, NameHierarchy nameHierarchy)
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

Node* Storage::addFileNode(const FilePath& filePath)
{
	NameHierarchy fileNameHierarchy;
	fileNameHierarchy.push(std::make_shared<NameElement>(filePath.fileName()));
	Node* fileNode = addNodeHierarchy(Node::NODE_FILE, fileNameHierarchy);

	if (!fileNode->getComponent<TokenComponentFilePath>())
	{
		fileNode->addComponentFilePath(std::make_shared<TokenComponentFilePath>(filePath));
	}

	return fileNode;
}

Node* Storage::findFileNode(const FilePath& filePath) const
{
	SearchNode* searchNode = m_tokenIndex.getNode(filePath.fileName());
	if (!searchNode || searchNode->getTokenIds().size() != 1)
	{
		return nullptr;
	}

	Node* fileNode = m_graph.getNodeById(searchNode->getFirstTokenId());
	if (!fileNode->isType(Node::NODE_FILE))
	{
		LOG_ERROR("Node is not of type file.");
		return nullptr;
	}

	return fileNode;
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
		LOG_ERROR_STREAM(<< "Cannot assign access" << access << " to node " << node->getFullName() << " because it's no child.");
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

	NameHierarchy nameHierarchy = typeUsage.dataType->getTypeNameHierarchy();

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

bool Storage::getQuerySearchResults(const std::string& query, const std::string& word, SearchResults* results) const
{
	std::string q = query;
	bool isCommand = false;

	switch (QueryOperator::getOperatorType(q.back()))
	{
	case QueryOperator::OPERATOR_AND:
	case QueryOperator::OPERATOR_OR:
	case QueryOperator::OPERATOR_NOT:
		q.pop_back();
		return false;

	case QueryOperator::OPERATOR_HAS:
		q.pop_back();
		q.append("'member'");
		break;

	case QueryOperator::OPERATOR_SUB:
		q.pop_back();
		break;

	case QueryOperator::OPERATOR_COMMAND:
		isCommand = true;
	case QueryOperator::OPERATOR_NONE:
	case QueryOperator::OPERATOR_TOKEN:
	case QueryOperator::OPERATOR_GROUP_OPEN:
	case QueryOperator::OPERATOR_GROUP_CLOSE:
		break;
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
			if (searchNodes.size() == 1 && !isCommand)
			{
				SearchResults res = node->runFuzzySearch(word);
				results->insert(res.begin(), res.end());
			}
			else
			{
				SearchResults res = node->runFuzzySearchOnSelf(word);
				results->insert(res.begin(), res.end());
			}
		}
		else if (searchNodes.size() == 1 && !isCommand)
		{
			for (const std::shared_ptr<SearchNode>& child : node->getChildren())
			{
				child->addResultsRecursive(results, 1, child.get());
			}
		}
		else
		{
			node->addResults(results, 1, node);
		}
	}

	return true;
}

void Storage::addDependingFilePathsAndRemoveFileNodesRecursive(Node* fileNode, std::set<FilePath>* filePaths)
{
	bool inserted = filePaths->insert(fileNode->getComponent<TokenComponentFilePath>()->getFilePath()).second;
	if (!inserted)
	{
		return;
	}

	while (true)
	{
		Edge* edge = fileNode->findEdgeOfType(Edge::EDGE_INCLUDE);
		if (!edge)
		{
			break;
		}

		Node* dependantNode = nullptr;
		if (edge->getTo() == fileNode)
		{
			dependantNode = edge->getFrom();
		}

		m_graph.removeEdge(edge);

		if (dependantNode)
		{
			addDependingFilePathsAndRemoveFileNodesRecursive(dependantNode, filePaths);
		}
	}

	removeNodeIfUnreferenced(fileNode);
}

void Storage::removeNodeIfUnreferenced(Node* node)
{
	Id tokenId = node->getId();
	SearchNode* searchNode = m_tokenIndex.getNode(node->getTokenComponentName()->getSearchNode());

	Node* parentNode = node->getParentNode();

	bool removed = m_graph.removeNodeIfUnreferencedRecursive(node);

	if (!removed)
	{
		return;
	}

	if (searchNode)
	{
		searchNode->removeTokenId(tokenId);
		m_tokenIndex.removeNodeIfUnreferencedRecursive(searchNode);
	}

	if (parentNode)
	{
		removeNodeIfUnreferenced(parentNode);
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
