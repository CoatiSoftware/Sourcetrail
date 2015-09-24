#include "data/Storage.h"

#include <sstream>
#include <queue>

#include "utility/file/FileSystem.h"
#include "utility/logging/logging.h"
#include "utility/utility.h"
#include "utility/utilityString.h"

#include "data/graph/token_component/TokenComponentAggregation.h"
#include "data/graph/Graph.h"
#include "data/location/TokenLocation.h"
#include "data/location/TokenLocationFile.h"
#include "data/location/TokenLocationLine.h"
#include "data/parser/ParseFunction.h"
#include "data/parser/ParseLocation.h"
#include "data/parser/ParseTypeUsage.h"
#include "data/parser/ParseVariable.h"
#include "data/type/DataType.h"
#include "settings/ApplicationSettings.h"

Storage::Storage(const FilePath& dbPath)
	: m_sqliteStorage(dbPath.str())
{
}

Storage::~Storage()
{
}

void Storage::clear()
{
	m_sqliteStorage.clear();

	clearCaches();

	m_errorMessages.clear();
	m_errorLocationCollection.clear();
}

void Storage::clearCaches()
{
	m_tokenIndex.clear();
	m_fileNodeIds.clear();
	m_hierarchyCache.clear();
}

void Storage::clearFileElements(const std::set<FilePath>& filePaths)
{
	for (const FilePath& filePath: filePaths)
	{
		clearFileElements(filePath);
	}
}

void Storage::clearFileElements(const FilePath& filePath)
{
	Id fileId = m_sqliteStorage.getFileByName(filePath.fileName()).id;
	if (fileId != 0)
	{
		m_sqliteStorage.removeElementsWithLocationInFile(fileId);
		m_sqliteStorage.removeFile(fileId);
	}
}

std::set<FilePath> Storage::getDependingFilePaths(const std::set<FilePath>& filePaths)
{
	std::set<FilePath> dependingFilePaths;
	for (const FilePath& filePath: filePaths)
	{
		std::set<FilePath> dependingFilePathsSubset = getDependingFilePaths(filePath);
		dependingFilePaths.insert(dependingFilePathsSubset.begin(), dependingFilePathsSubset.end());
	}
	return dependingFilePaths;
}

std::set<FilePath> Storage::getDependingFilePaths(const FilePath& filePath)
{
	std::set<FilePath> dependingFilePaths;

	Id fileNodeId = getFileNodeId(filePath);
	std::vector<StorageEdge> incomingEdges = m_sqliteStorage.getEdgesByTargetType(
		fileNodeId, Edge::typeToInt(Edge::EDGE_INCLUDE)
	);
	for (StorageEdge incomingEdge: incomingEdges)
	{
		Id dependingFileId = incomingEdge.sourceNodeId;
		FilePath dependingFilePath = FilePath(m_sqliteStorage.getFileById(dependingFileId).filePath);

		dependingFilePaths.insert(dependingFilePath);

		std::set<FilePath> dependingFilePathsSubset = getDependingFilePaths(dependingFilePath);
		dependingFilePaths.insert(dependingFilePathsSubset.begin(), dependingFilePathsSubset.end());
	}

	return dependingFilePaths;
}

void Storage::removeUnusedNames()
{
	m_sqliteStorage.removeUnusedNameHierarchyElements();

	clearCaches();
}

void Storage::logGraph() const
{
}

void Storage::logLocations() const
{
}

void Storage::logIndex() const
{
}

void Storage::logStats() const
{
}

void Storage::startParsing()
{
}

void Storage::finishParsing()
{
	buildSearchIndex();
	buildHierarchyCache();
}

void Storage::prepareParsingFile()
{
	m_sqliteStorage.beginTransaction();
}

void Storage::finishParsingFile()
{
	m_sqliteStorage.commitTransaction();
}

void Storage::onError(const ParseLocation& location, const std::string& message)
{
	log("ERROR", message, location);

	if (!location.isValid())
	{
		return;
	}

	bool duplicate = false;
	TokenLocationFile* file = m_errorLocationCollection.findTokenLocationFileByPath(location.filePath);

	if (file)
	{
		file->forEachStartTokenLocation(
			[&](TokenLocation* loc)
			{
				if (loc->getLineNumber() == location.startLineNumber &&
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
			getErrorCount(), errorId, location.filePath,
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

	Id typedefNodeId = addNodeHierarchy(Node::NODE_TYPEDEF, nameHierarchy);
	addSourceLocation(typedefNodeId, location);
	addAccess(typedefNodeId, access);

	Id underlyingTypeNodeId = addNodeHierarchy(Node::NODE_UNDEFINED_TYPE, underlyingType.dataType->getTypeNameHierarchy());
	addEdge(typedefNodeId, underlyingTypeNodeId, Edge::EDGE_TYPEDEF_OF, location);

	return typedefNodeId;
}

Id Storage::onClassParsed(
	const ParseLocation& location, const NameHierarchy& nameHierarchy, AccessType access,
	const ParseLocation& scopeLocation
){
	log("class", nameHierarchy.getFullName(), location);

	Id nodeId = addNodeHierarchy(scopeLocation.isValid() ? Node::NODE_CLASS : Node::NODE_UNDEFINED_TYPE, nameHierarchy);

	addSourceLocation(nodeId, location);
	addSourceLocation(nodeId, scopeLocation, true);

	addAccess(nodeId, access);

	return nodeId;
}

Id Storage::onStructParsed(
	const ParseLocation& location, const NameHierarchy& nameHierarchy, AccessType access,
	const ParseLocation& scopeLocation
){
	log("struct", nameHierarchy.getFullName(), location);

	Id nodeId = addNodeHierarchy(Node::NODE_STRUCT, nameHierarchy);

	addSourceLocation(nodeId, location);
	addSourceLocation(nodeId, scopeLocation, true);

	addAccess(nodeId, access);

	return nodeId;
}

Id Storage::onGlobalVariableParsed(const ParseLocation& location, const ParseVariable& variable)
{
	log("global", variable.getFullName(), location);

	Id nodeId = addNodeHierarchy(Node::NODE_GLOBAL_VARIABLE, variable.nameHierarchy);
	addSourceLocation(nodeId, location);

	Id typeNodeId = addNodeHierarchy(Node::NODE_UNDEFINED_TYPE, variable.type.dataType->getTypeNameHierarchy());
	addEdge(nodeId, typeNodeId, Edge::EDGE_TYPE_OF, location);

	return nodeId;
}

Id Storage::onFieldParsed(const ParseLocation& location, const ParseVariable& variable, AccessType access)
{
	log("field", variable.getFullName(), location);

	Id nodeId = addNodeHierarchy(Node::NODE_FIELD, variable.nameHierarchy);
	addSourceLocation(nodeId, location);
	addAccess(nodeId, access);

	Id typeNodeId = addNodeHierarchy(Node::NODE_UNDEFINED_TYPE, variable.type.dataType->getTypeNameHierarchy());
	addEdge(nodeId, typeNodeId, Edge::EDGE_TYPE_OF, variable.type.location);

	return nodeId;
}

Id Storage::onFunctionParsed(
	const ParseLocation& location, const ParseFunction& function, const ParseLocation& scopeLocation
){
	log("function", function.getFullName(), location);

	Id nodeId = addNodeHierarchyWithDistinctSignature(Node::NODE_FUNCTION, function);

	Id returnTypeNodeId = addNodeHierarchy(Node::NODE_UNDEFINED_TYPE, function.returnType.dataType->getTypeNameHierarchy());
	addEdge(nodeId, returnTypeNodeId, Edge::EDGE_TYPE_USAGE, function.returnType.location);
	// addEdge(nodeId, returnTypeNodeId, Edge::EDGE_RETURN_TYPE_OF, function.returnType.location);

	for (size_t i = 0; i < function.parameters.size(); i++)
	{
		Id parameternTypeNodeId = addNodeHierarchy(Node::NODE_UNDEFINED_TYPE, function.parameters[i].dataType->getTypeNameHierarchy());
		addEdge(nodeId, parameternTypeNodeId, Edge::EDGE_TYPE_USAGE, function.parameters[i].location);
		// addEdge(nodeId, parameternTypeNodeId, Edge::EDGE_PARAMETER_TYPE_OF, function.parameters[i].location);
	}

	addSourceLocation(nodeId, location);
	addSourceLocation(nodeId, scopeLocation, true);

	return nodeId;
}

Id Storage::onMethodParsed(
	const ParseLocation& location, const ParseFunction& method, AccessType access, AbstractionType abstraction,
	const ParseLocation& scopeLocation
){
	log("method", method.getFullName(), location);

	Id nodeId = addNodeHierarchyWithDistinctSignature(Node::NODE_METHOD, method);
	addSourceLocation(nodeId, location);
	addSourceLocation(nodeId, scopeLocation, true);
	addAccess(nodeId, access);

	Id returnTypeNodeId = addNodeHierarchy(Node::NODE_UNDEFINED_TYPE, method.returnType.dataType->getTypeNameHierarchy());
	addEdge(nodeId, returnTypeNodeId, Edge::EDGE_TYPE_USAGE, method.returnType.location);
	// addEdge(nodeId, returnTypeNodeId, Edge::EDGE_RETURN_TYPE_OF, method.returnType.location);

	for (size_t i = 0; i < method.parameters.size(); i++)
	{
		Id parameternTypeNodeId = addNodeHierarchy(Node::NODE_UNDEFINED_TYPE, method.parameters[i].dataType->getTypeNameHierarchy());
		addEdge(nodeId, parameternTypeNodeId, Edge::EDGE_TYPE_USAGE, method.parameters[i].location);
		// addEdge(nodeId, parameternTypeNodeId, Edge::EDGE_PARAMETER_TYPE_OF, method.parameters[i].location);
	}

	return nodeId;
}

Id Storage::onNamespaceParsed(
	const ParseLocation& location, const NameHierarchy& nameHierarchy, const ParseLocation& scopeLocation
){
	log("namespace", nameHierarchy.getFullName(), location);

	Id nodeId = addNodeHierarchy(Node::NODE_NAMESPACE, nameHierarchy);

	addSourceLocation(nodeId, location);
	addSourceLocation(nodeId, scopeLocation, true);

	return nodeId;
}

Id Storage::onEnumParsed(
	const ParseLocation& location, const NameHierarchy& nameHierarchy, AccessType access,
	const ParseLocation& scopeLocation
){
	log("enum", nameHierarchy.getFullName(), location);

	Id nodeId = addNodeHierarchy(Node::NODE_ENUM, nameHierarchy);

	addSourceLocation(nodeId, location);
	addSourceLocation(nodeId, scopeLocation, true);
	addAccess(nodeId, access);

	return nodeId;
}

Id Storage::onEnumConstantParsed(const ParseLocation& location, const NameHierarchy& nameHierarchy)
{
	log("enum constant", nameHierarchy.getFullName(), location);

	Id nodeId = addNodeHierarchy(Node::NODE_ENUM_CONSTANT, nameHierarchy);

	addSourceLocation(nodeId, location);

	return nodeId;
}

Id Storage::onInheritanceParsed(
	const ParseLocation& location, const NameHierarchy& childNameHierarchy,
	const NameHierarchy& parentNameHierarchy, AccessType access
){
	log("inheritance", childNameHierarchy.getFullName() + " : " + parentNameHierarchy.getFullName(), location);

	Id childNodeId = addNodeHierarchy(Node::NODE_UNDEFINED_TYPE, childNameHierarchy);
	Id parentNodeId = addNodeHierarchy(Node::NODE_UNDEFINED_TYPE, parentNameHierarchy);

	Id edgeId = addEdge(childNodeId, parentNodeId, Edge::EDGE_INHERITANCE, location);

	return edgeId;
}

Id Storage::onMethodOverrideParsed(
	const ParseLocation& location, const ParseFunction& base, const ParseFunction& overrider)
{
	log("override", base.getFullName() + " -> " + overrider.getFullName(), location);

	Id baseNodeId = addNodeHierarchyWithDistinctSignature(Node::NODE_UNDEFINED_FUNCTION, base); // TODO: call this overridden
	Id overriderNodeId = addNodeHierarchyWithDistinctSignature(Node::NODE_UNDEFINED_FUNCTION, overrider);

	Id edgeId = addEdge(overriderNodeId, baseNodeId, Edge::EDGE_OVERRIDE, location);

	return edgeId;
}

Id Storage::onCallParsed(const ParseLocation& location, const ParseFunction& caller, const ParseFunction& callee)
{
	log("call", caller.getFullName() + " -> " + callee.getFullName(), location);

	Id callerNodeId = addNodeHierarchyWithDistinctSignature(Node::NODE_UNDEFINED_FUNCTION, caller);
	Id calleeNodeId = addNodeHierarchyWithDistinctSignature(Node::NODE_UNDEFINED_FUNCTION, callee);

	Id edgeId = addEdge(callerNodeId, calleeNodeId, Edge::EDGE_CALL, location);

	return edgeId;
}

Id Storage::onCallParsed(const ParseLocation& location, const ParseVariable& caller, const ParseFunction& callee)
{
	log("call", caller.getFullName() + " -> " + callee.getFullName(), location);

	Id callerNodeId = addNodeHierarchy(Node::NODE_UNDEFINED, caller.nameHierarchy);
	Id calleeNodeId = addNodeHierarchyWithDistinctSignature(Node::NODE_UNDEFINED_FUNCTION, callee);

	Id edgeId = addEdge(callerNodeId, calleeNodeId, Edge::EDGE_CALL, location);

	return edgeId;
}

Id Storage::onFieldUsageParsed(
	const ParseLocation& location, const ParseFunction& user, const NameHierarchy& usedNameHierarchy
){
	log("field usage", user.getFullName() + " -> " + usedNameHierarchy.getFullName(), location);

	Id userNodeId = addNodeHierarchyWithDistinctSignature(Node::NODE_UNDEFINED_FUNCTION, user);
	Id usedNodeId = addNodeHierarchy(Node::NODE_UNDEFINED_VARIABLE, usedNameHierarchy);

	Id edgeId = addEdge(userNodeId, usedNodeId, Edge::EDGE_USAGE, location);

	return edgeId;
}

Id Storage::onFieldUsageParsed(
	const ParseLocation& location, const ParseVariable& user, const NameHierarchy& usedNameHierarchy
){
	log("global usage", user.getFullName() + " -> " + usedNameHierarchy.getFullName(), location);

	Id userNodeId = addNodeHierarchy(Node::NODE_UNDEFINED_FUNCTION, user.nameHierarchy);
	Id usedNodeId = addNodeHierarchy(Node::NODE_UNDEFINED_VARIABLE, usedNameHierarchy);

	Id edgeId = addEdge(userNodeId, usedNodeId, Edge::EDGE_USAGE, location);

	return edgeId;
}

Id Storage::onGlobalVariableUsageParsed( // or static variable used
	const ParseLocation& location, const ParseFunction& user, const NameHierarchy& usedNameHierarchy
){
	log("global usage", user.getFullName() + " -> " + usedNameHierarchy.getFullName(), location);

	Id userNodeId = addNodeHierarchyWithDistinctSignature(Node::NODE_UNDEFINED_FUNCTION, user);
	Id usedNodeId = addNodeHierarchy(Node::NODE_UNDEFINED_VARIABLE, usedNameHierarchy);

	Id edgeId = addEdge(userNodeId, usedNodeId, Edge::EDGE_USAGE, location);

	return edgeId;
}

Id Storage::onGlobalVariableUsageParsed(
	const ParseLocation& location, const ParseVariable& user, const NameHierarchy& usedNameHierarchy)
{
	log("global usage", user.getFullName() + " -> " + usedNameHierarchy.getFullName(), location);

	Id userNodeId = addNodeHierarchy(Node::NODE_UNDEFINED_VARIABLE, user.nameHierarchy);
	Id usedNodeId = addNodeHierarchy(Node::NODE_UNDEFINED_VARIABLE, usedNameHierarchy);

	Id edgeId = addEdge(userNodeId, usedNodeId, Edge::EDGE_USAGE, location);

	return edgeId;
}

Id Storage::onEnumConstantUsageParsed(
		const ParseLocation& location, const ParseFunction& user, const NameHierarchy& usedNameHierarchy
){
	log("enum constant usage", user.getFullName() + " -> " + usedNameHierarchy.getFullName(), location);

	Id userNodeId = addNodeHierarchyWithDistinctSignature(Node::NODE_UNDEFINED_FUNCTION, user);
	Id usedNodeId = addNodeHierarchy(Node::NODE_UNDEFINED_VARIABLE, usedNameHierarchy);

	Id edgeId = addEdge(userNodeId, usedNodeId, Edge::EDGE_USAGE, location);

	return edgeId;
}

Id Storage::onEnumConstantUsageParsed(
		const ParseLocation& location, const ParseVariable& user, const NameHierarchy& usedNameHierarchy
){
	log("enum constant usage", user.getFullName() + " -> " + usedNameHierarchy.getFullName(), location);

	Id userNodeId = addNodeHierarchy(Node::NODE_UNDEFINED_FUNCTION, user.nameHierarchy);
	Id usedNodeId = addNodeHierarchy(Node::NODE_UNDEFINED_VARIABLE, usedNameHierarchy);

	Id edgeId = addEdge(userNodeId, usedNodeId, Edge::EDGE_USAGE, location);

	return edgeId;
}

Id Storage::onTypeUsageParsed(const ParseTypeUsage& typeUsage, const ParseFunction& function) // check if type has valid location
{
	log("type usage", function.getFullName() + " -> " + typeUsage.dataType->getRawTypeName(), typeUsage.location);

	if (!typeUsage.location.isValid())
	{
		return 0;
	}

	Id functionNodeId = addNodeHierarchyWithDistinctSignature(Node::NODE_UNDEFINED_FUNCTION, function);
	Id typeNodeId = addNodeHierarchy(Node::NODE_UNDEFINED_TYPE, typeUsage.dataType->getTypeNameHierarchy());

	Id edgeId = addEdge(functionNodeId, typeNodeId, Edge::EDGE_TYPE_USAGE, typeUsage.location);

	return edgeId;
}

Id Storage::onTypeUsageParsed(const ParseTypeUsage& typeUsage, const ParseVariable& variable)
{
	log("type usage", variable.getFullName() + " -> " + typeUsage.dataType->getRawTypeName(), typeUsage.location);

	if (!typeUsage.location.isValid())
	{
		return 0;
	}

	Id functionNodeId = addNodeHierarchy(Node::NODE_UNDEFINED, variable.nameHierarchy);
	Id typeNodeId = addNodeHierarchy(Node::NODE_UNDEFINED_TYPE, typeUsage.dataType->getTypeNameHierarchy());

	Id edgeId = addEdge(functionNodeId, typeNodeId, Edge::EDGE_TYPE_USAGE, typeUsage.location);

	return edgeId;
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

	Id argumentNodeId = addNodeHierarchy(Node::NODE_UNDEFINED_TYPE, argumentNameHierarchy);
	// does not need a source location because this type that is already defined (and therefore has a location).

	Id templateNodeId = addNodeHierarchy(Node::NODE_UNDEFINED_TYPE, templateNameHierarchy);

	addEdge(argumentNodeId, templateNodeId, Edge::EDGE_TEMPLATE_ARGUMENT_OF, location);

	return argumentNodeId;
}

Id Storage::onTemplateDefaultArgumentTypeParsed(
	const ParseTypeUsage& defaultArgumentTypeUsage,
	const NameHierarchy& templateArgumentTypeNameHierarchy // actually this is the template parameter???
){
	log(
		"template default argument",
		defaultArgumentTypeUsage.dataType->getTypeNameHierarchy().getFullName() + " -> " + templateArgumentTypeNameHierarchy.getFullName(),
		defaultArgumentTypeUsage.location
	);

	Id defaultArgumentNodeId = addNodeHierarchy(Node::NODE_UNDEFINED_TYPE, defaultArgumentTypeUsage.dataType->getTypeNameHierarchy());
	// does not need a source location because this type that is already defined (and therefore has a location).

	Id argumentNodeId = addNodeHierarchy(Node::NODE_UNDEFINED_TYPE, templateArgumentTypeNameHierarchy);

	addEdge(defaultArgumentNodeId, argumentNodeId, Edge::EDGE_TEMPLATE_DEFAULT_ARGUMENT_OF, defaultArgumentTypeUsage.location);

	return defaultArgumentNodeId;
}

Id Storage::onTemplateRecordParameterTypeParsed(
	const ParseLocation& location, const NameHierarchy& templateParameterTypeNameHierarchy,
	const NameHierarchy& templateRecordNameHierarchy
){
	log("template record type parameter", templateParameterTypeNameHierarchy.getFullName(), location);

	Id parameterNodeId = addNodeHierarchy(Node::NODE_TEMPLATE_PARAMETER_TYPE, templateParameterTypeNameHierarchy);
	addSourceLocation(parameterNodeId, location, false);

	Id recordNodeId = addNodeHierarchy(Node::NODE_UNDEFINED_TYPE, templateRecordNameHierarchy);

	addEdge(parameterNodeId, recordNodeId, Edge::EDGE_TEMPLATE_PARAMETER_OF, location);

	return parameterNodeId;
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

	Id specializedNodeId = addNodeHierarchy(specializedRecordNodeType, specializedRecordNameHierarchy);
	addSourceLocation(specializedNodeId, location, false);

	Id recordNodeId = addNodeHierarchy(Node::NODE_UNDEFINED_TYPE, specializedFromNameHierarchy);

	addEdge(specializedNodeId, recordNodeId, Edge::EDGE_TEMPLATE_SPECIALIZATION_OF, location);

	return specializedNodeId;
}

Id Storage::onTemplateFunctionParameterTypeParsed(
	const ParseLocation& location, const NameHierarchy& templateParameterTypeNameHierarchy, const ParseFunction function
){
	log("template function type parameter", templateParameterTypeNameHierarchy.getFullName(), location);

	Id parameterNodeId = addNodeHierarchy(Node::NODE_TEMPLATE_PARAMETER_TYPE, templateParameterTypeNameHierarchy);
	addSourceLocation(parameterNodeId, location, false);

	Id functionNodeId = addNodeHierarchyWithDistinctSignature(Node::NODE_UNDEFINED_FUNCTION, function);

	addEdge(parameterNodeId, functionNodeId, Edge::EDGE_TEMPLATE_PARAMETER_OF, location);

	return parameterNodeId;
}

Id Storage::onTemplateFunctionSpecializationParsed(
	const ParseLocation& location, const ParseFunction specializedFunction, const ParseFunction templateFunction
){
	log("function template specialization", specializedFunction.getFullName(), location);

	Id specializedNodeId = addNodeHierarchyWithDistinctSignature(Node::NODE_FUNCTION, specializedFunction);
	addSourceLocation(specializedNodeId, location, false);

	Id functionNodeId = addNodeHierarchyWithDistinctSignature(Node::NODE_UNDEFINED_FUNCTION, templateFunction);

	addEdge(specializedNodeId, functionNodeId, Edge::EDGE_TEMPLATE_SPECIALIZATION_OF, location);

	return specializedNodeId;
}

Id Storage::onFileParsed(const FileInfo& fileInfo)
{
	log("file", fileInfo.path.str(), ParseLocation());

	const std::string fileName = fileInfo.path.fileName();

	Id nameHierarchyElementId = m_sqliteStorage.getNameHierarchyElementIdByName(fileName);
	if (nameHierarchyElementId == 0)
	{
		nameHierarchyElementId = m_sqliteStorage.addNameHierarchyElement(fileName);
	}

	Id fileNodeId = getFileNodeId(fileInfo.path);
	if (fileNodeId == 0)
	{
		fileNodeId = m_sqliteStorage.addFile(
			nameHierarchyElementId,
			fileInfo.path.str(),
			utility::timeToString(fileInfo.lastWriteTime)
		);
	}

	NameHierarchy nameHierarchy;
	nameHierarchy.push(std::make_shared<NameElement>(fileName));

	return fileNodeId;
}

Id Storage::onFileIncludeParsed(const ParseLocation& location, const FileInfo& fileInfo, const FileInfo& includedFileInfo)
{
	log("include", includedFileInfo.path.str(), location);

	const Id fileNodeId = onFileParsed(fileInfo);
	const Id includedFileNodeId = onFileParsed(includedFileInfo);

	addEdge(fileNodeId, includedFileNodeId, Edge::EDGE_INCLUDE, location);

	return fileNodeId;
}

Id Storage::getIdForNodeWithName(const std::string& fullName) const // use name hierarchy here
{
	std::vector<std::string> nameParts = utility::splitToVector(fullName, "::");

	Id parentId = 0;
	for (size_t i = 0; i < nameParts.size(); i++)
	{
		Id currentId = 0;
		if (parentId == 0)
		{
			currentId = m_sqliteStorage.getNameHierarchyElementIdByName(nameParts[i]);
		}
		else
		{
			currentId = m_sqliteStorage.getNameHierarchyElementIdByName(nameParts[i], parentId);
		}

		parentId = currentId;

		if (currentId == 0)
		{
			break;
		}
	}

	return m_sqliteStorage.getNodeByNameId(parentId).id;
}

Id Storage::getIdForEdgeWithName(const std::string& name) const
{
	Edge::EdgeType type;
	std::string sourceName;
	std::string targetName;

	Edge::splitName(name, &type, &sourceName, &targetName);

	int sourceId = getIdForNodeWithName(sourceName);
	int targetId = getIdForNodeWithName(targetName);
	return m_sqliteStorage.getEdgeBySourceTargetType(sourceId, targetId, type).id;
}

std::vector<FileInfo> Storage::getInfoOnAllFiles() const
{
	std::vector<FileInfo> fileInfos;

	std::vector<StorageFile> storageFiles = m_sqliteStorage.getAllFiles();
	for (size_t i = 0; i < storageFiles.size(); i++)
	{
		boost::posix_time::ptime modificationTime = boost::posix_time::not_a_date_time;
		if (storageFiles[i].modificationTime != "not-a-date-time")
		{
			modificationTime = boost::posix_time::time_from_string(storageFiles[i].modificationTime);
		}
		fileInfos.push_back(FileInfo(
			FilePath(storageFiles[i].filePath),
			modificationTime
		));
	}

	return fileInfos;
}

std::string Storage::getNameForNodeWithId(Id nodeId) const
{
	Id nameHierarchyElementId = m_sqliteStorage.getNameHierarchyElementIdByNodeId(nodeId);
	return m_sqliteStorage.getNameHierarchyById(nameHierarchyElementId).getFullName();
	// return m_tokenIndex.getNameHierarchyForTokenId(nodeId).getFullName();
}

Node::NodeType Storage::getNodeTypeForNodeWithId(Id nodeId) const
{
	return Node::intToType(m_sqliteStorage.getNodeById(nodeId).type);
}

std::vector<SearchMatch> Storage::getAutocompletionMatches(const std::string& query, const std::string& word) const
{
	SearchResults tokenResults = m_tokenIndex.runFuzzySearch(word);

	std::vector<SearchMatch> matches = SearchIndex::getMatches(tokenResults, word);
	SearchMatch::log(matches, word);

	for (SearchMatch& match : matches)
	{
		if (!match.tokenIds.size())
		{
			match.searchType = SearchMatch::SEARCH_COMMAND;
			continue;
		}

		Id elementId = *(match.tokenIds.cbegin());
		if (m_sqliteStorage.isNode(elementId))
		{
			match.nodeType = Node::intToType(m_sqliteStorage.getNodeById(elementId).type);
			match.typeName = Node::getTypeString(match.nodeType);
		}
		else
		{
			match.typeName = Edge::getTypeString(Edge::intToType(m_sqliteStorage.getEdgeById(elementId).type));
		}

		match.searchType = SearchMatch::SEARCH_TOKEN;
	}

	return matches;
}

std::shared_ptr<Graph> Storage::getGraphForActiveTokenIds(const std::vector<Id>& tokenIds) const
{
	std::shared_ptr<Graph> g = std::make_shared<Graph>();
	Graph* graph = g.get();

	if (tokenIds.size() == 1)
	{
		const Id elementId = tokenIds[0];

		if (m_sqliteStorage.isNode(elementId))
		{
			const StorageNode node = m_sqliteStorage.getNodeById(elementId);

			addNodeAndAllChildrenToGraph(getLastVisibleParentNodeId(node.id), graph);

			std::vector<StorageEdge> edges = m_sqliteStorage.getEdgesBySourceOrTargetId(node.id);

			for (size_t i = 0; i < edges.size(); i++)
			{
				if (Edge::intToType(edges[i].type) != Edge::EDGE_MEMBER)
				{
					addEdgeAndAllChildrenToGraph(edges[i].id, graph);
				}
			}

			addAggregationEdgesToGraph(elementId, graph);
		}
		else
		{
			addEdgeAndAllChildrenToGraph(elementId, graph);
		}
	}
	else if (tokenIds.size() > 1)
	{
		for (size_t i = 0; i < tokenIds.size(); i++)
		{
			const Id elementId = tokenIds[i];

			if (m_sqliteStorage.isNode(elementId))
			{
				addNodeAndAllChildrenToGraph(getLastVisibleParentNodeId(elementId), graph);
			}
			else
			{
				addEdgeAndAllChildrenToGraph(elementId, graph);
			}
		}
	}

	addComponentAccessToGraph(graph);

	return g;
}

std::vector<Id> Storage::getActiveTokenIdsForTokenIds(const std::vector<Id>& tokenIds) const
{
	std::vector<Id> activeIds;

	for (Id id : tokenIds)
	{
		if (m_sqliteStorage.isNode(id))
		{
			m_hierarchyCache.addFirstVisibleChildIdsForNodeId(id, &activeIds);
		}
		else
		{
			activeIds.push_back(id);
		}
	}

	return activeIds;
}

// TODO: rename: getActiveElementIdsForId; TODO: make separate function for declarationId
std::vector<Id> Storage::getActiveTokenIdsForId(Id tokenId, Id* declarationId) const
{
	std::vector<Id> activeTokenIds;

	if (!(m_sqliteStorage.isEdge(tokenId) || m_sqliteStorage.isNode(tokenId)))
	{
		return activeTokenIds;
	}

	activeTokenIds.push_back(tokenId);

	if (m_sqliteStorage.isNode(tokenId))
	{
		*declarationId = tokenId;
		std::vector<StorageEdge> storageEdges = m_sqliteStorage.getEdgesByTargetId(tokenId);
		for (size_t i = 0; i < storageEdges.size(); i++)
		{
			activeTokenIds.push_back(storageEdges[i].id);
		}
	}

	return activeTokenIds;
}

std::vector<Id> Storage::getNodeIdsForLocationIds(const std::vector<Id>& locationIds) const
{
	std::vector<Id> nodeIds;

	for (Id locationId : locationIds)
	{
		Id elementId = m_sqliteStorage.getElementIdByLocationId(locationId);

		StorageEdge edge = m_sqliteStorage.getEdgeById(elementId);
		if (edge.id != 0) // here we test if location is an edge.
		{
			nodeIds.push_back(edge.targetNodeId);
		}
		else
		{
			nodeIds.push_back(elementId);
		}
	}

	return nodeIds;
}

std::vector<Id> Storage::getTokenIdsForMatches(const std::vector<SearchMatch>& matches) const
{
	std::set<Id> idSet;
	for (const SearchMatch& match : matches)
	{
		std::vector<SearchMatch> ms = getAutocompletionMatches("", match.fullName);

		for (size_t i = 0; i < ms.size(); i++)
		{
			utility::append(idSet, ms[i].tokenIds);
			break;
		}
	}

	std::vector<Id> ids;
	for (std::set<Id>::const_iterator it = idSet.begin(); it != idSet.end(); it++)
	{
		ids.push_back(*it);
	}

	return ids;
}

Id Storage::getTokenIdForFileNode(const FilePath& filePath) const
{
	return m_sqliteStorage.getFileByName(filePath.fileName()).id;
}

std::vector<Id> Storage::getTokenIdsForAggregationEdge(Id sourceId, Id targetId) const
{
	std::vector<Id> edgeIds;

	std::vector<Id> aggregationEndpointsA = getAllChildNodeIds(sourceId);
	std::set<Id> aggregationEndpointsB;
	aggregationEndpointsB.insert(targetId);
	for (const Id targetChildId: getAllChildNodeIds(targetId))
	{
		aggregationEndpointsB.insert(targetChildId);
	}

	for (size_t i = 0; i < aggregationEndpointsA.size(); i++)
	{
		std::vector<StorageEdge> outgoingEdges = m_sqliteStorage.getEdgesBySourceId(aggregationEndpointsA[i]);
		for (size_t j = 0; j < outgoingEdges.size(); j++)
		{
			if (aggregationEndpointsB.find(outgoingEdges[j].targetNodeId) != aggregationEndpointsB.end())
			{
				edgeIds.push_back(outgoingEdges[j].id);
			}
		}

		std::vector<StorageEdge> incomingEdges = m_sqliteStorage.getEdgesByTargetId(aggregationEndpointsA[i]);
		for (size_t j = 0; j < incomingEdges.size(); j++)
		{
			if (aggregationEndpointsB.find(incomingEdges[j].sourceNodeId) != aggregationEndpointsB.end())
			{
				edgeIds.push_back(incomingEdges[j].id);
			}
		}
	}

	return edgeIds;
}

std::shared_ptr<TokenLocationCollection> Storage::getTokenLocationsForTokenIds(const std::vector<Id>& tokenIds) const
{
	std::shared_ptr<TokenLocationCollection> collection = std::make_shared<TokenLocationCollection>();

	for (Id elementId: tokenIds)
	{
		if (m_sqliteStorage.isFile(elementId))
		{
			StorageFile storageFile = m_sqliteStorage.getFileById(elementId);
			collection->addTokenLocationFileAsPlainCopy(m_sqliteStorage.getTokenLocationsForFile(storageFile.filePath).get());
		}
		else
		{
			std::vector<StorageSourceLocation> locations = m_sqliteStorage.getTokenLocationsForElementId(elementId);
			for (size_t i = 0; i < locations.size(); i++)
			{
				// TODO: optimize: fileNodeId to name in a separate map
				const StorageSourceLocation& location = locations[i];
				StorageFile storageFile = m_sqliteStorage.getFileById(location.fileNodeId);

				TokenLocation* loc = collection->addTokenLocation(
					location.id,
					location.elementId,
					storageFile.filePath,
					location.startLine,
					location.startCol,
					location.endLine,
					location.endCol
				);

				if (loc)
				{
					loc->setType(location.isScope ? TokenLocation::LOCATION_SCOPE : TokenLocation::LOCATION_TOKEN);
				}
			}
		}
	}

	return collection;
}

std::shared_ptr<TokenLocationCollection> Storage::getTokenLocationsForLocationIds(const std::vector<Id>& locationIds) const
{
	std::shared_ptr<TokenLocationCollection> collection = std::make_shared<TokenLocationCollection>();

	for (size_t i = 0; i < locationIds.size(); i++)
	{
		StorageSourceLocation location = m_sqliteStorage.getSourceLocationById(locationIds[i]);
		collection->addTokenLocation(
			location.id,
			location.elementId,
			m_sqliteStorage.getFileById(location.fileNodeId).filePath, // TODO: optimize: only once per file!
			location.startLine,
			location.startCol,
			location.endLine,
			location.endCol)->setType(location.isScope ? TokenLocation::LOCATION_SCOPE : TokenLocation::LOCATION_TOKEN
		);
	}

	return collection;
}

std::shared_ptr<TokenLocationFile> Storage::getTokenLocationsForFile(const std::string& filePath) const
{
	std::shared_ptr<TokenLocationFile> locationFile = m_sqliteStorage.getTokenLocationsForFile(filePath);
	locationFile->isWholeCopy = true;
	return locationFile;
}

std::shared_ptr<TokenLocationFile> Storage::getTokenLocationsForLinesInFile(
		const std::string& filePath, uint firstLineNumber, uint lastLineNumber
) const
{
	std::shared_ptr<TokenLocationFile> ret = std::make_shared<TokenLocationFile>(filePath);

	std::shared_ptr<TokenLocationFile> locationFile = m_sqliteStorage.getTokenLocationsForFile(filePath);
	if (!locationFile->getTokenLocationLines().size())
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

	std::shared_ptr<TokenLocationFile> locationFile = m_sqliteStorage.getTokenLocationsForFile(filePath);
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

const SearchIndex& Storage::getSearchIndex() const
{
	return m_tokenIndex;
}

Id Storage::addNodeHierarchy(Node::NodeType nodeType, NameHierarchy nameHierarchy, bool distinct)
{
	addNameHierarchyElements(nameHierarchy);

	Id parentNameHierarchyElementId = 0;
	Id parentNodeId = 0;
	for (size_t i = 0; i < nameHierarchy.size(); i++)
	{
		bool lastName = (i == nameHierarchy.size() - 1);

		Node::NodeType type = (lastName ? nodeType : Node::NODE_UNDEFINED);

		Id nameHierarchyElementId =
			m_sqliteStorage.getNameHierarchyElementIdByName(nameHierarchy[i]->getFullName(), parentNameHierarchyElementId);

		const StorageNode node = m_sqliteStorage.getNodeByNameId(nameHierarchyElementId);
		Id nodeId = node.id;

		if (nodeId == 0 || (lastName && distinct))
		{
			nodeId = m_sqliteStorage.addNode(Node::typeToInt(type), nameHierarchyElementId);

			if (parentNodeId != 0)
			{
				addEdge(parentNodeId, nodeId, Edge::EDGE_MEMBER);
			}
		}
		else if (lastName) // Update the type of the last node if the new type is more specific.
		{
			Node::NodeType storedType = Node::intToType(node.type);
			if (type > storedType)
			{
				m_sqliteStorage.setNodeType(Node::typeToInt(type), nodeId);
			}
		}

		parentNameHierarchyElementId = nameHierarchyElementId;
		parentNodeId = nodeId;
	}

	return parentNodeId;
}

Id Storage::addNodeHierarchyWithDistinctSignature(Node::NodeType type, const ParseFunction& function)
{
	std::string signature = ParserClient::functionSignatureStr(function);
	Id nodeId = m_sqliteStorage.getNodeIdBySignature(signature);

	if (!nodeId)
	{
		nodeId = addNodeHierarchy(type, function.nameHierarchy, true);
		m_sqliteStorage.addSignature(nodeId, signature);
	}

	return nodeId;
}

Id Storage::addNameHierarchyElements(NameHierarchy nameHierarchy)
{
	Id parentId = 0;
	bool nodeMayExist = true;

	for (size_t i = 0; i < nameHierarchy.size(); i++)
	{
		const std::string elementName = nameHierarchy[i]->getFullName();
		int nodeId = 0;

		if (nodeMayExist)
		{
			nodeId = m_sqliteStorage.getNameHierarchyElementIdByName(elementName, parentId);
		}
		else
		{
			nodeId = 0;
		}

		if (nodeId == 0)
		{
			nodeId = m_sqliteStorage.addNameHierarchyElement(elementName, parentId);
			nodeMayExist = false;
		}

		parentId = nodeId;
	}

	return parentId;
}

int Storage::addSourceLocation(int elementNodeId, const ParseLocation& location, bool isScope)
{
	if (!location.isValid())
	{
		return 0;
	}

	if (location.filePath.empty())
	{
		LOG_ERROR("no filename set!");
		return 0;
	}
	else
	{
		Id fileNodeId = getFileNodeId(location.filePath);

		if (!fileNodeId)
		{
			LOG_ERROR("Can't create source location, file node does not exist for: " + location.filePath.str());
			return 0;
		}

		int locationId = m_sqliteStorage.addSourceLocation(
			elementNodeId, fileNodeId, location.startLineNumber, location.startColumnNumber,
			location.endLineNumber, location.endColumnNumber, isScope
		);
		return locationId;
	}
}

Id Storage::addEdge(Id sourceNodeId, Id targetNodeId, Edge::EdgeType type)
{
	Id edgeId = m_sqliteStorage.getEdgeBySourceTargetType(sourceNodeId, targetNodeId, type).id;

	if (!edgeId)
	{
		edgeId = m_sqliteStorage.addEdge(type, sourceNodeId, targetNodeId);
	}

	return edgeId;
}

Id Storage::addEdge(Id sourceNodeId, Id targetNodeId, Edge::EdgeType type, ParseLocation location)
{
	Id edgeId = addEdge(sourceNodeId, targetNodeId, type);

	addSourceLocation(edgeId, location, false);

	return edgeId;
}

Id Storage::getFileNodeId(const FilePath& filePath)
{
	std::map<FilePath, Id>::const_iterator it = m_fileNodeIds.find(filePath);

	if (it != m_fileNodeIds.end())
	{
		return it->second;
	}

	if (filePath.empty())
	{
		LOG_ERROR("No file path set");
		return 0;
	}

	StorageFile storageFile = m_sqliteStorage.getFileByPath(filePath.str());

	if (storageFile.id == 0)
	{
		return 0;
	}

	m_fileNodeIds.emplace(filePath, storageFile.id);

	return storageFile.id;
}

Id Storage::getLastVisibleParentNodeId(const Id nodeId) const
{
	return m_hierarchyCache.getLastVisibleParentNodeId(nodeId);
}

std::vector<Id> Storage::getAllChildNodeIds(const Id nodeId) const
{
	std::vector<Id> childNodeIds;
	std::vector<Id> edgeIds;

	m_hierarchyCache.addAllChildIdsForNodeId(nodeId, &childNodeIds, &edgeIds);

	return childNodeIds;
}

void Storage::addEdgeAndAllChildrenToGraph(const Id edgeId, Graph* graph) const
{
	StorageEdge storageEdge = m_sqliteStorage.getEdgeById(edgeId);

	Node* sourceNode = graph->getNodeById(storageEdge.sourceNodeId);
	Node* targetNode = graph->getNodeById(storageEdge.targetNodeId);

	if (!sourceNode)
	{
		addNodeAndAllChildrenToGraph(getLastVisibleParentNodeId(storageEdge.sourceNodeId), graph);
		sourceNode = graph->getNodeById(storageEdge.sourceNodeId);
	}

	if (!targetNode)
	{
		addNodeAndAllChildrenToGraph(getLastVisibleParentNodeId(storageEdge.targetNodeId), graph);
		targetNode = graph->getNodeById(storageEdge.targetNodeId);
	}

	graph->createEdge(edgeId, Edge::intToType(storageEdge.type), sourceNode, targetNode);
}

Node* Storage::addNodeAndAllChildrenToGraph(const Id nodeId, Graph* graph) const
{
	Node* node = graph->getNodeById(nodeId);
	if (node)
	{
		return node;
	}

	std::vector<Id> nodeIdsToAdd;
	std::vector<Id> edgeIdsToAdd;

	nodeIdsToAdd.push_back(nodeId);

	m_hierarchyCache.addAllChildIdsForNodeId(nodeId, &nodeIdsToAdd, &edgeIdsToAdd);

	addNodesToGraph(nodeIdsToAdd, graph);
	addEdgesToGraph(edgeIdsToAdd, graph);

	return graph->getNodeById(nodeId);
}

void Storage::addAggregationEdgesToGraph(const Id nodeId, Graph* graph) const
{
	struct EdgeInfo
	{
		Id edgeId;
		bool forward;
	};

	// build aggregation edges:
	// get all children of the active node
	std::vector<Id> childNodeIds = getAllChildNodeIds(nodeId);

	// get all edges of the children
	std::map<Id, std::vector<EdgeInfo>> connectedNodeIds;

	std::vector<StorageEdge> outgoingEdges = m_sqliteStorage.getEdgesBySourceIds(childNodeIds);
	for (size_t j = 0; j < outgoingEdges.size(); j++)
	{
		EdgeInfo edgeInfo;
		edgeInfo.edgeId = outgoingEdges[j].id;
		edgeInfo.forward = true;
		connectedNodeIds[outgoingEdges[j].targetNodeId].push_back(edgeInfo);
	}

	std::vector<StorageEdge> incomingEdges = m_sqliteStorage.getEdgesByTargetIds(childNodeIds);
	for (size_t j = 0; j < incomingEdges.size(); j++)
	{
		EdgeInfo edgeInfo;
		edgeInfo.edgeId = incomingEdges[j].id;
		edgeInfo.forward = false;
		connectedNodeIds[incomingEdges[j].sourceNodeId].push_back(edgeInfo);
	}

	// get all parent nodes of all connected nodes (up to last level except namespace/undefined)
	Id nodeParentNodeId = getLastVisibleParentNodeId(nodeId);

	std::map<Id, std::vector<EdgeInfo>> connectedParentNodeIds;
	for (const std::pair<Id, std::vector<EdgeInfo>>& p : connectedNodeIds)
	{
		Id parentNodeId = getLastVisibleParentNodeId(p.first);

		if (parentNodeId != nodeParentNodeId)
		{
			utility::append(connectedParentNodeIds[parentNodeId], p.second);
		}
	}

	// add hierarchies for these parents and create aggregation edges between parents and active node
	Node* sourceNode = graph->getNodeById(nodeId);

	for (const std::pair<Id, std::vector<EdgeInfo>> p : connectedParentNodeIds)
	{
		const int aggregationTargetNodeId = p.first;

		Node* targetNode = graph->getNodeById(aggregationTargetNodeId);
		if (!targetNode)
		{
			targetNode = addNodeAndAllChildrenToGraph(aggregationTargetNodeId, graph);
		}

		std::shared_ptr<TokenComponentAggregation> componentAggregation = std::make_shared<TokenComponentAggregation>();
		for (const EdgeInfo& edgeInfo: p.second)
		{
			componentAggregation->addAggregationId(edgeInfo.edgeId, edgeInfo.forward);
		}

		Edge* edge = graph->createEdge(
			*componentAggregation->getAggregationIds().begin(),
			Edge::EDGE_AGGREGATION,
			sourceNode,
			targetNode
		);

		edge->addComponentAggregation(componentAggregation);
	}
}

void Storage::addNodesToGraph(const std::vector<Id> nodeIds, Graph* graph) const
{
	std::vector<StorageNode> storageNodes = m_sqliteStorage.getNodesByIds(nodeIds);

	for (const StorageNode& storageNode : storageNodes)
	{
		graph->createNode(
			storageNode.id,
			Node::intToType(storageNode.type),
			m_tokenIndex.getNameHierarchyForTokenId(storageNode.id)
		);
	}
}

void Storage::addEdgesToGraph(const std::vector<Id> edgeIds, Graph* graph) const
{
	std::vector<StorageEdge> storageEdges = m_sqliteStorage.getEdgesByIds(edgeIds);
	for (const StorageEdge& storageEdge : storageEdges)
	{
		Node* sourceNode = graph->getNodeById(storageEdge.sourceNodeId);
		Node* targetNode = graph->getNodeById(storageEdge.targetNodeId);

		if (sourceNode && targetNode)
		{
			graph->createEdge(storageEdge.id, Edge::intToType(storageEdge.type), sourceNode, targetNode);
		}
		else
		{
			LOG_ERROR("Can't add edge because nodes are not present");
		}
	}
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

void Storage::addAccess(const Id nodeId, ParserClient::AccessType access)
{
	if (access == ACCESS_NONE)
	{
		return;
	}

	std::vector<StorageEdge> memberEdges = m_sqliteStorage.getEdgesByTargetType(nodeId, Edge::EDGE_MEMBER);
	if (memberEdges.size() != 1)
	{
		LOG_ERROR_STREAM(<< "Cannot assign access" << access << " to node id " << nodeId << " because it's no child.");
		return;
	}

	m_sqliteStorage.addComponentAccess(memberEdges[0].id, convertAccessType(access));
}

void Storage::addComponentAccessToGraph(Graph* graph) const
{
	std::vector<Id> memberEdgeIds;

	graph->forEachEdge(
		[&memberEdgeIds](Edge* edge)
		{
			if (!edge->isType(Edge::EDGE_MEMBER))
			{
				return;
			}

			memberEdgeIds.push_back(edge->getId());
		}
	);

	std::vector<StorageComponentAccess> accesses = m_sqliteStorage.getComponentAccessByMemberEdgeIds(memberEdgeIds);
	for (const StorageComponentAccess& access : accesses)
	{
		if (access.memberEdgeId && access.type)
		{
			graph->getEdgeById(access.memberEdgeId)->addComponentAccess(
				std::make_shared<TokenComponentAccess>(TokenComponentAccess::intToType(access.type)));
		}
	}
}

void Storage::buildSearchIndex()
{
	for (StorageNode node: m_sqliteStorage.getAllNodes())
	{
		m_tokenIndex.addTokenId(m_tokenIndex.addNode(m_sqliteStorage.getNameHierarchyById(node.nameId)), node.id);
	}
}

void Storage::buildHierarchyCache()
{
	std::vector<StorageEdge> memberEdges = m_sqliteStorage.getEdgesByType(Edge::typeToInt(Edge::EDGE_MEMBER));

	for (const StorageEdge& edge : memberEdges)
	{
		bool isVisible = !(Node::intToType(m_sqliteStorage.getNodeById(edge.sourceNodeId).type) & Node::NODE_NOT_VISIBLE);
		m_hierarchyCache.createConnection(edge.id, edge.sourceNodeId, edge.targetNodeId, isVisible);
	}
}

void Storage::log(std::string type, std::string str, const ParseLocation& location) const
{
	LOG_INFO_STREAM(
		<< type << ": " << str << " <" << location.filePath.str() << " "
		<< location.startLineNumber << ":" << location.startColumnNumber << " "
		<< location.endLineNumber << ":" << location.endColumnNumber << ">"
	);
}
