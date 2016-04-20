#include "data/Storage.h"

#include <sstream>
#include <queue>

#include "utility/file/FileSystem.h"
#include "utility/logging/logging.h"
#include "utility/messaging/type/MessageClearErrorCount.h"
#include "utility/messaging/type/MessageShowErrors.h"
#include "utility/TimePoint.h"
#include "utility/utility.h"
#include "utility/utilityString.h"
#include "utility/Version.h"
#include "utility/Cache.h"
#include "utility/utilityString.h"

#include "data/graph/token_component/TokenComponentAggregation.h"
#include "data/graph/token_component/TokenComponentSignature.h"
#include "data/graph/Graph.h"
#include "data/location/TokenLocation.h"
#include "data/location/TokenLocationFile.h"
#include "data/location/TokenLocationLine.h"
#include "data/parser/ParseLocation.h"
#include "data/type/DataType.h"
#include "settings/ApplicationSettings.h"

Storage::Storage(const FilePath& dbPath)
	: m_sqliteStorage(dbPath.str())
{
}

Storage::~Storage()
{
}

Version Storage::getVersion() const
{
	return m_sqliteStorage.getVersion();
}

bool Storage::init()
{
	m_commandIndex.addNode(0, NameHierarchy(SearchMatch::getCommandName(SearchMatch::COMMAND_ALL)));
	m_commandIndex.addNode(0, NameHierarchy(SearchMatch::getCommandName(SearchMatch::COMMAND_ERROR)));
	m_commandIndex.finishSetup();

	return m_sqliteStorage.init();
}

void Storage::clear()
{
	m_sqliteStorage.clear();

	clearCaches();
}

void Storage::clearCaches()
{
	m_elementIndex.clear();
	m_fileNodeIds.clear();
	m_hierarchyCache.clear();
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

	std::vector<StorageEdge> incomingEdges = m_sqliteStorage.getEdgesByTargetType(
		getFileNodeId(filePath), Edge::typeToInt(Edge::EDGE_INCLUDE)
	);
	for (const StorageEdge& incomingEdge: incomingEdges)
	{
		FilePath dependingFilePath = getFileNodePath(incomingEdge.sourceNodeId);
		dependingFilePaths.insert(dependingFilePath);

		std::set<FilePath> dependingFilePathsSubset = getDependingFilePaths(dependingFilePath);
		dependingFilePaths.insert(dependingFilePathsSubset.begin(), dependingFilePathsSubset.end());
	}

	return dependingFilePaths;
}

void Storage::clearFileElements(const std::vector<FilePath>& filePaths)
{
	std::vector<Id> fileNodeIds;

	for (const FilePath& path : filePaths)
	{
		fileNodeIds.push_back(getFileNodeId(path));
	}

	if (fileNodeIds.size())
	{
		m_sqliteStorage.removeElementsWithLocationInFiles(fileNodeIds);
		m_sqliteStorage.removeElements(fileNodeIds);

		m_sqliteStorage.removeErrorsInFiles(filePaths);
	}
}

void Storage::removeUnusedNames() // maybe rename this function. look for callers first.
{
//	m_sqliteStorage.removeUnusedNameHierarchyElements();

	clearCaches();
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

void Storage::logStats() const
{
	std::stringstream ss;
	StorageStats stats = getStorageStats();

	ss << "\nGraph:\n";
	ss << "\t" << stats.nodeCount << " Nodes\n";
	ss << "\t" << stats.edgeCount << " Edges\n";

	ss << "\nCode:\n";
	ss << "\t" << stats.fileCount << " Files\n";
	ss << "\t" << stats.fileLOCCount << " Lines of Code\n";

	ss << "\nErrors:\n";
	ss << "\t" << stats.errorCount.total << " Errors\n";
	ss << "\t" << stats.errorCount.fatal << " Fatal Errors\n";

	LOG_WARNING(ss.str());
}

void Storage::startParsing()
{
	MessageClearErrorCount().dispatch();

	m_sqliteStorage.setVersion(Version::getApplicationVersion());
}

void Storage::finishParsing()
{
	buildSearchIndex();
	buildHierarchyCache();
}

void Storage::injectData(std::shared_ptr<IntermediateStorage> injectedStorage)
{
	int totalErrorCount = getErrorCount().total;

	injectedStorage->transferToStorage(m_sqliteStorage);

	if (totalErrorCount != getErrorCount().total)
	{
		MessageShowErrors msg(getErrorCount());
		msg.setSendAsTask(false);
		msg.dispatch();
	}
}

Id Storage::getIdForNodeWithNameHierarchy(const NameHierarchy& nameHierarchy) const
{
	return m_sqliteStorage.getNodeBySerializedName(NameHierarchy::serialize(nameHierarchy)).id;
}

Id Storage::getIdForEdge(
	Edge::EdgeType type, const NameHierarchy& fromNameHierarchy, const NameHierarchy& toNameHierarchy
) const
{
	Id sourceId = getIdForNodeWithNameHierarchy(fromNameHierarchy);
	Id targetId = getIdForNodeWithNameHierarchy(toNameHierarchy);
	return m_sqliteStorage.getEdgeBySourceTargetType(sourceId, targetId, type).id;
}

Id Storage::getIdForFirstNode() const
{
	return m_sqliteStorage.getFirstNode().id;
}

NameHierarchy Storage::getNameHierarchyForNodeWithId(Id nodeId) const
{
	return NameHierarchy::deserialize(m_sqliteStorage.getNodeById(nodeId).serializedName);
}

Node::NodeType Storage::getNodeTypeForNodeWithId(Id nodeId) const
{
	return Node::intToType(m_sqliteStorage.getNodeById(nodeId).type);
}

std::vector<SearchMatch> Storage::getAutocompletionMatches(const std::string& query) const
{
	std::vector<SearchResult> commandResults = m_commandIndex.search(query, 0);

	const size_t maxResultCount = 100;
	std::vector<SearchResult> elementResults = m_elementIndex.search(query, maxResultCount);
	std::sort(elementResults.begin(), elementResults.end(), [](
		const SearchResult& a,
		const SearchResult& b)
		{
			// should a be ranked higher than b?
			if (a.score > b.score)
			{
				return true;
			}
			else if (a.score == b.score)
			{

				if (a.text.size() < b.text.size())
				{
					return true;
				}
				else if (a.text.size() == b.text.size())
				{
					// move uppercase letters to higher ascii range
					std::string sA = utility::switchCases(a.text);
					std::string sB = utility::switchCases(b.text);
					return (sA.compare(sB) < 0);
				}
			}
			return false;
		}
	);

	std::vector<SearchResult> results;
	utility::append(results, commandResults);
	utility::append(results, elementResults);

	std::vector<SearchMatch> matches;
	for (size_t i = 0; i < results.size(); i++)
	{
		SearchMatch match;

		if (results[i].elementIds.size() > 0)
		{
			StorageNode firstNode(0, 0, "", 0);
			for (std::set<Id>::const_iterator itElementIds = results[i].elementIds.begin();
				itElementIds != results[i].elementIds.end();
				itElementIds++)
			{
				Id elementId = *itElementIds;
				if (elementId != 0)
				{
					StorageNode node = m_sqliteStorage.getNodeById(elementId);
					match.nameHierarchies.push_back(NameHierarchy::deserialize(node.serializedName));

					if (firstNode.id == 0)
					{
						firstNode = node;
					}
				}
			}

			match.text = results[i].text;
			match.indices = results[i].indices;

			if (firstNode.id != 0)
			{
				match.nodeType = Node::intToType(firstNode.type);
				match.typeName = Node::getTypeString(match.nodeType);

				if (intToDefinitionType(firstNode.definitionType) == DEFINITION_NONE && match.nodeType != Node::NODE_UNDEFINED)
				{
					match.typeName = "undefined " + match.typeName;
				}
				match.searchType = SearchMatch::SEARCH_TOKEN;
			}
			else
			{
				match.searchType = SearchMatch::SEARCH_COMMAND;
				match.typeName = "command";
			}
		}

		matches.push_back(match);
	}

	return matches;
}

std::vector<SearchMatch> Storage::getSearchMatchesForTokenIds(const std::vector<Id>& elementIds) const
{
	// todo: what if all these elements share the same node in the searchindex?
	// In that case there should be only one search match.
	std::vector<SearchMatch> matches;

	for (Id elementId : elementIds)
	{
		SearchMatch match;

		if (m_sqliteStorage.isFile(elementId))
		{
			match.nodeType = Node::NODE_FILE;
		}
		else if (m_sqliteStorage.isNode(elementId))
		{
			StorageNode node = m_sqliteStorage.getNodeById(elementId);
			match.nodeType = Node::intToType(node.type);
		}
		else
		{
			continue;
		}

		NameHierarchy nameHierarchy = NameHierarchy::deserialize(m_sqliteStorage.getNodeById(elementId).serializedName);
		match.text = nameHierarchy.getQualifiedName();
		match.nameHierarchies.push_back(nameHierarchy.getQualifiedName());
		match.searchType = SearchMatch::SEARCH_TOKEN;

		matches.push_back(match);
	}

	return matches;
}

std::shared_ptr<Graph> Storage::getGraphForAll() const
{
	std::shared_ptr<Graph> graph = std::make_shared<Graph>();

	std::vector<Id> tokenIds;
	for (StorageNode node: m_sqliteStorage.getAllNodes())
	{
		if (intToDefinitionType(node.definitionType) == DEFINITION_EXPLICIT &&
			(!m_hierarchyCache.isChildOfVisibleNodeOrInvisible(node.id) ||
			Node::intToType(node.type) == Node::NODE_NAMESPACE))
		{
			tokenIds.push_back(node.id);
		}
	}

	addNodesToGraph(tokenIds, graph.get());

	return graph;
}

std::shared_ptr<Graph> Storage::getGraphForActiveTokenIds(const std::vector<Id>& tokenIds) const
{
	std::shared_ptr<Graph> g = std::make_shared<Graph>();
	Graph* graph = g.get();

	std::vector<Id> ids(tokenIds);
	bool isNamespace = false;

	std::vector<Id> nodeIds;
	std::vector<Id> edgeIds;
	bool addAggregations = false;

	if (tokenIds.size() == 1)
	{
		const Id elementId = tokenIds[0];
		StorageNode node = m_sqliteStorage.getNodeById(elementId);

		if (node.id > 0)
		{
			if (Node::intToType(node.type) == Node::NODE_NAMESPACE)
			{
				ids.clear();
				m_hierarchyCache.addFirstChildIdsForNodeId(elementId, &ids);

				isNamespace = true;
			}
			else
			{
				nodeIds.push_back(elementId);

				std::vector<StorageEdge> edges = m_sqliteStorage.getEdgesBySourceOrTargetId(elementId);
				for (const StorageEdge& edge : edges)
				{
					if (Edge::intToType(edge.type) != Edge::EDGE_MEMBER)
					{
						edgeIds.push_back(edge.id);
					}
				}

				addAggregations = true;
			}
		}
		else if (m_sqliteStorage.isEdge(elementId))
		{
			edgeIds.push_back(elementId);
		}
	}

	if (ids.size() >= 1 || isNamespace)
	{
		std::vector<StorageNode> nodes = m_sqliteStorage.getNodesByIds(ids);
		for (const StorageNode& node : nodes)
		{
			if (node.id > 0 && (!isNamespace || intToDefinitionType(node.definitionType) == DEFINITION_EXPLICIT))
			{
				nodeIds.push_back(node.id);
			}
		}

		if (nodeIds.size() != ids.size())
		{
			std::vector<StorageEdge> edges = m_sqliteStorage.getEdgesByIds(ids);
			for (const StorageEdge& edge : edges)
			{
				if (edge.id > 0)
				{
					edgeIds.push_back(edge.id);
				}
			}
		}
	}

	if (isNamespace)
	{
		addNodesToGraph(nodeIds, graph);
	}
	else
	{
		addNodesWithChildrenAndEdgesToGraph(nodeIds, edgeIds, graph);
	}

	if (addAggregations)
	{
		addAggregationEdgesToGraph(tokenIds[0], graph);
	}

	addComponentAccessToGraph(graph);

	return g;
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

		std::vector<StorageEdge> incomingEdges = m_sqliteStorage.getEdgesByTargetId(tokenId);
		for (size_t i = 0; i < incomingEdges.size(); i++)
		{
			activeTokenIds.push_back(incomingEdges[i].id);
		}
	}

	return activeTokenIds;
}

std::vector<Id> Storage::getNodeIdsForLocationIds(const std::vector<Id>& locationIds) const
{
	std::set<Id> edgeIds;
	std::set<Id> nodeIds;
	std::set<Id> implicitNodeIds;

	for (Id locationId : locationIds)
	{
		Id elementId = m_sqliteStorage.getElementIdByLocationId(locationId);

		StorageEdge edge = m_sqliteStorage.getEdgeById(elementId);
		if (edge.id != 0) // here we test if location is an edge.
		{
			edgeIds.insert(edge.targetNodeId);
		}
		else if(m_sqliteStorage.isNode(elementId))
		{
			StorageNode node = m_sqliteStorage.getNodeById(elementId);
			if (node.id != 0)
			{
				if (intToDefinitionType(node.definitionType) == DEFINITION_IMPLICIT)
				{
					implicitNodeIds.insert(elementId);
				}
				else
				{
					nodeIds.insert(elementId);
				}
			}
		}
	}

	if (nodeIds.size() == 0)
	{
		nodeIds = implicitNodeIds;
	}

	if (nodeIds.size())
	{
		return utility::toVector(nodeIds);
	}

	return utility::toVector(edgeIds);
}

std::vector<Id> Storage::getLocalSymbolIdsForLocationIds(const std::vector<Id>& locationIds) const
{
	std::set<Id> localSymbolIds;

	for (Id locationId : locationIds)
	{
		Id elementId = m_sqliteStorage.getElementIdByLocationId(locationId);

		if (m_sqliteStorage.getNodeById(elementId).id == 0 && m_sqliteStorage.getEdgeById(elementId).id == 0)
		{
			localSymbolIds.insert(elementId);
		}
	}

	return utility::toVector(localSymbolIds);
}

std::vector<Id> Storage::getTokenIdsForMatches(const std::vector<SearchMatch>& matches) const
{
	std::set<Id> idSet;
	for (const SearchMatch& match : matches)
	{
		for (size_t i = 0; i < match.nameHierarchies.size(); i++)
		{
			idSet.insert(m_sqliteStorage.getNodeBySerializedName(NameHierarchy::serialize(match.nameHierarchies[i])).id);
		}
	}

	std::vector<Id> ids;
	for (std::set<Id>::const_iterator it = idSet.begin(); it != idSet.end(); it++)
	{
		if (*it != 0)
		{
			ids.push_back(*it);
		}
	}

	return ids;
}

Id Storage::getTokenIdForFileNode(const FilePath& filePath) const
{
	return m_sqliteStorage.getFileByPath(filePath.str()).id;
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

	std::vector<Id> fileIds;
	std::vector<Id> nonFileIds;
	for (size_t i = 0; i < tokenIds.size(); i++)
	{
		if (m_sqliteStorage.isFile(tokenIds[i]))
		{
			fileIds.push_back(tokenIds[i]);
		}
		else
		{
			nonFileIds.push_back(tokenIds[i]);
		}
	}

	for (Id fileId: fileIds)
	{
		StorageFile storageFile = m_sqliteStorage.getFileById(fileId);
		collection->addTokenLocationFileAsPlainCopy(m_sqliteStorage.getTokenLocationsForFile(storageFile.filePath).get());
	}

	Cache<Id, std::string> filePathCache(
		[this](Id id) -> std::string
		{
			return m_sqliteStorage.getFileById(id).filePath;
		}
	);

	std::vector<StorageSourceLocation> locations = m_sqliteStorage.getTokenLocationsForElementIds(nonFileIds);
	for (size_t i = 0; i < locations.size(); i++)
	{
		const StorageSourceLocation& location = locations[i];
		std::string filePath = filePathCache.getValue(location.fileNodeId);

		TokenLocation* loc = collection->addTokenLocation(
			location.id,
			location.elementId,
			filePath,
			location.startLine,
			location.startCol,
			location.endLine,
			location.endCol
		);

		if (loc)
		{
			loc->setType(intToLocationType(location.type));
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
			location.endCol
		)->setType(intToLocationType(location.type));
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
	return m_sqliteStorage.getTokenLocationsForFile(filePath)->getFilteredByLines(firstLineNumber, lastLineNumber);
}

TokenLocationCollection Storage::getErrorTokenLocations(std::vector<ErrorInfo>* errors) const
{
	TokenLocationCollection errorCollection;

	std::vector<StorageError> storageErrors = m_sqliteStorage.getAllErrors();
	for (size_t i = 0; i < storageErrors.size(); i++)
	{
		const StorageError& error = storageErrors[i];
		errorCollection.addTokenLocation(
			i, i, error.filePath, error.lineNumber, error.columnNumber, error.lineNumber, error.columnNumber);
		errors->push_back(ErrorInfo(error.message, error.filePath, i, error.fatal));
	}

	return errorCollection;
}

std::shared_ptr<TokenLocationFile> Storage::getCommentLocationsInFile(const FilePath& filePath) const
{
	std::shared_ptr<TokenLocationFile> file = std::make_shared<TokenLocationFile>(filePath);

	std::vector<StorageCommentLocation> storageLocations = m_sqliteStorage.getCommentLocationsInFile(filePath);
	for (size_t i = 0; i < storageLocations.size(); i++)
	{
		file->addTokenLocation(
			storageLocations[i].id,
			0, // comment token location has no element.
			storageLocations[i].startLine,
			storageLocations[i].startCol,
			storageLocations[i].endLine,
			storageLocations[i].endCol
		);
	}

	return file;
}

std::shared_ptr<TextAccess> Storage::getFileContent(const FilePath& filePath) const
{
	return m_sqliteStorage.getFileContentByPath(filePath.str());
}

TimePoint Storage::getFileModificationTime(const FilePath& filePath) const
{
	return TimePoint(m_sqliteStorage.getFileByPath(filePath.str()).modificationTime);
}

ErrorCountInfo Storage::getErrorCount() const
{
	return ErrorCountInfo(m_sqliteStorage.getAllErrors().size(), m_sqliteStorage.getFatalErrors().size());
}

StorageStats Storage::getStorageStats() const
{
	StorageStats stats;

	stats.nodeCount = m_sqliteStorage.getNodeCount();
	stats.edgeCount = m_sqliteStorage.getEdgeCount();

	stats.fileCount = m_sqliteStorage.getFileCount();
	stats.fileLOCCount = m_sqliteStorage.getFileLOCCount();

	stats.errorCount = getErrorCount();

	return stats;
}

Id Storage::getFileNodeId(const FilePath& filePath) const
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

FilePath Storage::getFileNodePath(Id fileId) const
{
	for (const std::pair<FilePath, Id>& p : m_fileNodeIds)
	{
		if (p.second == fileId)
		{
			return p.first;
		}
	}

	return m_sqliteStorage.getFileById(fileId).filePath;
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

void Storage::addNodesToGraph(const std::vector<Id>& nodeIds, Graph* graph) const
{
	if (nodeIds.size() == 0)
	{
		return;
	}

	std::vector<StorageNode> storageNodes = m_sqliteStorage.getNodesByIds(nodeIds);

	for (const StorageNode& storageNode : storageNodes)
	{
		NameHierarchy nameHierarchy = NameHierarchy::deserialize(storageNode.serializedName);

		Node::NodeType type = Node::intToType(storageNode.type);
		DefinitionType defType = intToDefinitionType(storageNode.definitionType);
		Node* node = graph->createNode(
			storageNode.id,
			type,
			nameHierarchy,
			defType != DEFINITION_NONE
		);

		if (defType == DEFINITION_IMPLICIT)
		{
			node->setImplicit(true);
		}
		else if (defType == DEFINITION_EXPLICIT)
		{
			node->setExplicit(true);
		}

		if (type == Node::NODE_FUNCTION || type == Node::NODE_METHOD)
		{
			std::string signatureString = nameHierarchy.getRawNameWithSignature();
			if (signatureString.size() > 0) // this should always be the case since functions and methods must have sigs.
			{
				node->addComponentSignature(
					std::make_shared<TokenComponentSignature>(signatureString)
				);
			}
		}
	}
}

void Storage::addEdgesToGraph(const std::vector<Id>& edgeIds, Graph* graph) const
{
	if (edgeIds.size() == 0)
	{
		return;
	}

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

void Storage::addNodesWithChildrenAndEdgesToGraph(const std::vector<Id>& nodeIds, const std::vector<Id>& edgeIds, Graph* graph) const
{
	std::set<Id> parentNodeIds;

	for (Id nodeId : nodeIds)
	{
		parentNodeIds.insert(getLastVisibleParentNodeId(nodeId));
	}

	if (edgeIds.size() > 0)
	{
		std::vector<StorageEdge> storageEdges = m_sqliteStorage.getEdgesByIds(edgeIds);
		for (const StorageEdge& storageEdge : storageEdges)
		{
			parentNodeIds.insert(getLastVisibleParentNodeId(storageEdge.sourceNodeId));
			parentNodeIds.insert(getLastVisibleParentNodeId(storageEdge.targetNodeId));
		}
	}

	std::vector<Id> allNodeIds;
	std::vector<Id> allEdgeIds = edgeIds;

	for (Id parentNodeId : parentNodeIds)
	{
		allNodeIds.push_back(parentNodeId);
		m_hierarchyCache.addAllChildIdsForNodeId(parentNodeId, &allNodeIds, &allEdgeIds);
	}

	addNodesToGraph(allNodeIds, graph);
	addEdgesToGraph(allEdgeIds, graph);
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
	if (childNodeIds.size() == 0)
	{
		return;
	}

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

	// add hierarchies of these parents
	std::vector<Id> nodeIdsToAdd;
	for (const std::pair<Id, std::vector<EdgeInfo>> p : connectedParentNodeIds)
	{
		const Id aggregationTargetNodeId = p.first;
		if (!graph->getNodeById(aggregationTargetNodeId))
		{
			nodeIdsToAdd.push_back(aggregationTargetNodeId);
		}
	}
	addNodesWithChildrenAndEdgesToGraph(nodeIdsToAdd, std::vector<Id>(), graph);

	// create aggregation edges between parents and active node
	Node* sourceNode = graph->getNodeById(nodeId);
	for (const std::pair<Id, std::vector<EdgeInfo>> p : connectedParentNodeIds)
	{
		const Id aggregationTargetNodeId = p.first;

		Node* targetNode = graph->getNodeById(aggregationTargetNodeId);
		if (!targetNode)
		{
			LOG_ERROR("Aggregation target node not present.");
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
		m_elementIndex.addNode(node.id, NameHierarchy::deserialize(node.serializedName));
	}
	m_elementIndex.finishSetup();
}

void Storage::buildHierarchyCache()
{
	std::vector<StorageEdge> memberEdges = m_sqliteStorage.getEdgesByType(Edge::typeToInt(Edge::EDGE_MEMBER));

	Cache<Id, Node::NodeType> nodeTypeCache([this](Id id){
		return Node::intToType(m_sqliteStorage.getNodeById(id).type);
	});

	for (const StorageEdge& edge : memberEdges)
	{
		bool isVisible = !(nodeTypeCache.getValue(edge.sourceNodeId) & Node::NODE_NOT_VISIBLE);
		m_hierarchyCache.createConnection(edge.id, edge.sourceNodeId, edge.targetNodeId, isVisible);
	}
}
