#include "data/storage/PersistentStorage.h"

#include <sstream>
#include <queue>

#include "data/graph/token_component/TokenComponentAccess.h"
#include "data/graph/token_component/TokenComponentAggregation.h"
#include "data/graph/token_component/TokenComponentFilePath.h"
#include "data/graph/token_component/TokenComponentInheritanceChain.h"
#include "data/graph/Graph.h"
#include "data/location/SourceLocationCollection.h"
#include "data/location/SourceLocationFile.h"
#include "data/parser/AccessKind.h"
#include "data/parser/ParseLocation.h"
#include "data/NodeTypeSet.h"
#include "settings/ApplicationSettings.h"
#include "utility/UnorderedCache.h"
#include "utility/file/FileInfo.h"
#include "utility/file/FilePath.h"
#include "utility/logging/logging.h"
#include "utility/messaging/type/MessageNewErrors.h"
#include "utility/messaging/type/MessageStatus.h"
#include "utility/text/TextAccess.h"
#include "utility/TextCodec.h"
#include "utility/TimeStamp.h"
#include "utility/tracing.h"
#include "utility/utility.h"

PersistentStorage::PersistentStorage(const FilePath& dbPath, const FilePath& bookmarkPath)
	: m_sqliteIndexStorage(dbPath)
	, m_sqliteBookmarkStorage(bookmarkPath)
{
	m_commandIndex.addNode(0, SearchMatch::getCommandName(SearchMatch::COMMAND_ALL));
	m_commandIndex.addNode(0, SearchMatch::getCommandName(SearchMatch::COMMAND_ERROR));

	for (const NodeType& nodeType : NodeTypeSet::all().getNodeTypes())
	{
		if (nodeType.hasSearchFilter())
		{
			m_commandIndex.addNode(0, nodeType.getReadableTypeWString());
		}
	}

	m_commandIndex.finishSetup();
}

PersistentStorage::~PersistentStorage()
{
}

Id PersistentStorage::addNode(const StorageNodeData& data)
{
	const StorageNode storedNode = m_sqliteIndexStorage.getNodeBySerializedName(data.serializedName);

	if (storedNode.id == 0)
	{
		return m_sqliteIndexStorage.addNode(data).id;
	}

	if (storedNode.type < data.type)
	{
		m_sqliteIndexStorage.setNodeType(data.type, storedNode.id);
		return storedNode.id;
	}

	return storedNode.id;
}

void PersistentStorage::addSymbol(const StorageSymbol& data)
{
	if (m_sqliteIndexStorage.getFirstById<StorageSymbol>(data.id).id == 0)
	{
		m_sqliteIndexStorage.addSymbol(data);
	}
}

void PersistentStorage::addFile(const StorageFile& data)
{
	const StorageFile storedFile = m_sqliteIndexStorage.getFirstById<StorageFile>(data.id);

	if (storedFile.id == 0)
	{
		m_sqliteIndexStorage.addFile(data);
	}

	if (!storedFile.complete && data.complete)
	{
		m_sqliteIndexStorage.setFileComplete(data.complete, storedFile.id);
	}
}

Id PersistentStorage::addEdge(const StorageEdgeData& data)
{
	const StorageEdge storedEdge = m_sqliteIndexStorage.getEdgeBySourceTargetType(data.sourceNodeId, data.targetNodeId, data.type);
	if (storedEdge.id == 0)
	{
		return m_sqliteIndexStorage.addEdge(data).id;
	}
	return storedEdge.id;
}

Id PersistentStorage::addLocalSymbol(const StorageLocalSymbolData& data)
{
	const StorageLocalSymbol storedLocalSymbol = m_sqliteIndexStorage.getLocalSymbolByName(data.name);
	if (storedLocalSymbol.id == 0)
	{
		return m_sqliteIndexStorage.addLocalSymbol(data).id;
	}
	return storedLocalSymbol.id;
}

Id PersistentStorage::addSourceLocation(const StorageSourceLocationData& data)
{
	return m_sqliteIndexStorage.addSourceLocation(data).id;
}

void PersistentStorage::addOccurrence(const StorageOccurrence& data)
{
	m_sqliteIndexStorage.addOccurrence(data);
}

void PersistentStorage::addComponentAccess(const StorageComponentAccessData& data)
{
	m_sqliteIndexStorage.addComponentAccess(data);
}

void PersistentStorage::addCommentLocation(const StorageCommentLocationData& data)
{
	m_sqliteIndexStorage.addCommentLocation(data);
}

void PersistentStorage::addError(const StorageErrorData& data)
{
	m_sqliteIndexStorage.addError(data);
}

void PersistentStorage::forEachNode(std::function<void(const StorageNode& /*data*/)> callback) const
{
	for (StorageNode& node: m_sqliteIndexStorage.getAll<StorageNode>())
	{
		callback(node);
	}
}

void PersistentStorage::forEachFile(std::function<void(const StorageFile& /*data*/)> callback) const
{
	for (StorageFile& file: m_sqliteIndexStorage.getAll<StorageFile>())
	{
		callback(file);
	}
}

void PersistentStorage::forEachSymbol(std::function<void(const StorageSymbol& /*data*/)> callback) const
{
	for (StorageSymbol& symbol: m_sqliteIndexStorage.getAll<StorageSymbol>())
	{
		callback(symbol);
	}
}

void PersistentStorage::forEachEdge(std::function<void(const StorageEdge& /*data*/)> callback) const
{
	for (StorageEdge& edge: m_sqliteIndexStorage.getAll<StorageEdge>())
	{
		callback(edge);
	}
}

void PersistentStorage::forEachLocalSymbol(std::function<void(const StorageLocalSymbol& /*data*/)> callback) const
{
	for (StorageLocalSymbol& localSymbol: m_sqliteIndexStorage.getAll<StorageLocalSymbol>())
	{
		callback(localSymbol);
	}
}

void PersistentStorage::forEachSourceLocation(std::function<void(const StorageSourceLocation& /*data*/)> callback) const
{
	for (StorageSourceLocation& sourceLocation: m_sqliteIndexStorage.getAll<StorageSourceLocation>())
	{
		callback(sourceLocation);
	}
}

void PersistentStorage::forEachOccurrence(std::function<void(const StorageOccurrence& /*data*/)> callback) const
{
	for (StorageOccurrence& occurrence: m_sqliteIndexStorage.getAll<StorageOccurrence>())
	{
		callback(occurrence);
	}
}

void PersistentStorage::forEachComponentAccess(std::function<void(const StorageComponentAccessData& /*data*/)> callback) const
{
	for (StorageComponentAccess& componentAccess: m_sqliteIndexStorage.getAll<StorageComponentAccess>())
	{
		callback(componentAccess);
	}
}

void PersistentStorage::forEachCommentLocation(std::function<void(const StorageCommentLocationData& /*data*/)> callback) const
{
	for (StorageCommentLocation& commentLocation: m_sqliteIndexStorage.getAll<StorageCommentLocation>())
	{
		callback(commentLocation);
	}
}

void PersistentStorage::forEachError(std::function<void(const StorageErrorData& /*data*/)> callback) const
{
	for (StorageError& error: m_sqliteIndexStorage.getAll<StorageError>())
	{
		callback(error);
	}
}

void PersistentStorage::startInjection()
{
	m_preInjectionErrorCount = getErrors().size();

	m_sqliteIndexStorage.beginTransaction();
}

void PersistentStorage::finishInjection()
{
	m_sqliteIndexStorage.commitTransaction();

	auto errors = getErrors();

	if (m_preInjectionErrorCount != errors.size())
	{
		MessageNewErrors(
			std::vector<ErrorInfo>(errors.begin() + m_preInjectionErrorCount, errors.end()),
			getErrorCount(errors)
		).dispatch();
	}
}

void PersistentStorage::setMode(const SqliteIndexStorage::StorageModeType mode)
{
	m_sqliteIndexStorage.setMode(mode);
}

FilePath PersistentStorage::getDbFilePath() const
{
	return m_sqliteIndexStorage.getDbFilePath();
}

bool PersistentStorage::isEmpty() const
{
	return m_sqliteIndexStorage.isEmpty();
}

bool PersistentStorage::isIncompatible() const
{
	return m_sqliteIndexStorage.isIncompatible();
}

std::string PersistentStorage::getProjectSettingsText() const
{
	return m_sqliteIndexStorage.getProjectSettingsText();
}

void PersistentStorage::setProjectSettingsText(std::string text)
{
	m_sqliteIndexStorage.setProjectSettingsText(text);
}

void PersistentStorage::setup()
{
	m_sqliteIndexStorage.setup();
	m_sqliteBookmarkStorage.setup();

	m_sqliteBookmarkStorage.migrateIfNecessary();
}

void PersistentStorage::clear()
{
	m_sqliteIndexStorage.clear();

	clearCaches();
}

void PersistentStorage::clearCaches()
{
	m_symbolIndex.clear();
	m_fileIndex.clear();

	m_fileNodeIds.clear();
	m_fileNodePaths.clear();
	m_fileNodeComplete.clear();
	m_symbolDefinitionKinds.clear();

	m_hierarchyCache.clear();
	m_fullTextSearchIndex.clear();
	m_fullTextSearchCodec = "";
}

std::set<FilePath> PersistentStorage::getReferenced(const std::set<FilePath>& filePaths)
{
	TRACE();
	std::set<FilePath> referenced;

	utility::append(referenced, getReferencedByIncludes(filePaths));
	utility::append(referenced, getReferencedByImports(filePaths));

	return referenced;
}

std::set<FilePath> PersistentStorage::getReferencing(const std::set<FilePath>& filePaths)
{
	TRACE();
	std::set<FilePath> referencing;

	utility::append(referencing, getReferencingByIncludes(filePaths));
	utility::append(referencing, getReferencingByImports(filePaths));

	return referencing;
}

void PersistentStorage::clearFileElements(const std::vector<FilePath>& filePaths, std::function<void(int)> updateStatusCallback)
{
	TRACE();

	const std::vector<Id> fileNodeIds = getFileNodeIds(filePaths);

	if (!fileNodeIds.empty())
	{
		m_sqliteIndexStorage.beginTransaction();
		m_sqliteIndexStorage.removeElementsWithLocationInFiles(fileNodeIds, updateStatusCallback);
		m_sqliteIndexStorage.removeElements(fileNodeIds);
		m_sqliteIndexStorage.removeErrorsInFiles(filePaths);
		m_sqliteIndexStorage.commitTransaction();
		updateStatusCallback(100);
	}
}

std::vector<FileInfo> PersistentStorage::getFileInfoForAllFiles() const
{
	TRACE();

	std::vector<FileInfo> fileInfos;
	for (StorageFile file : m_sqliteIndexStorage.getAll<StorageFile>())
	{
		boost::posix_time::ptime modificationTime = boost::posix_time::not_a_date_time;
		if (file.modificationTime != "not-a-date-time")
		{
			modificationTime = boost::posix_time::time_from_string(file.modificationTime);
		}

		fileInfos.push_back(
			FileInfo(
				FilePath(file.filePath),
				modificationTime
			)
		);
	}

	return fileInfos;
}

std::set<FilePath> PersistentStorage::getIncompleteFiles() const
{
	TRACE();

	std::set<FilePath> incompleteFiles;
	for (auto p : m_fileNodeComplete)
	{
		if (p.second == false)
		{
			incompleteFiles.insert(getFileNodePath(p.first));
		}
	}

	return incompleteFiles;
}

void PersistentStorage::buildCaches()
{
	TRACE();

	clearCaches();

	buildFilePathMaps();
	buildSearchIndex();
	buildMemberEdgeIdOrderMap();
	buildHierarchyCache();
}

void PersistentStorage::optimizeMemory()
{
	TRACE();

	m_sqliteIndexStorage.setVersion(m_sqliteIndexStorage.getStaticVersion());
	m_sqliteIndexStorage.setTime();
	m_sqliteIndexStorage.optimizeMemory();

	if (m_sqliteBookmarkStorage.isEmpty())
	{
		m_sqliteBookmarkStorage.setVersion(m_sqliteBookmarkStorage.getStaticVersion());
	}
	m_sqliteBookmarkStorage.optimizeMemory();
}

Id PersistentStorage::getNodeIdForFileNode(const FilePath& filePath) const
{
	return m_sqliteIndexStorage.getFileByPath(filePath.wstr()).id;
}

Id PersistentStorage::getNodeIdForNameHierarchy(const NameHierarchy& nameHierarchy) const
{
	return m_sqliteIndexStorage.getNodeBySerializedName(NameHierarchy::serialize(nameHierarchy)).id;
}

std::vector<Id> PersistentStorage::getNodeIdsForNameHierarchies(const std::vector<NameHierarchy> nameHierarchies) const
{
	std::vector<Id> nodeIds;
	for (const NameHierarchy& name : nameHierarchies)
	{
		Id nodeId = getNodeIdForNameHierarchy(name);
		if (nodeId)
		{
			nodeIds.push_back(nodeId);
		}
	}
	return nodeIds;
}

NameHierarchy PersistentStorage::getNameHierarchyForNodeId(Id nodeId) const
{
	TRACE();

	return NameHierarchy::deserialize(m_sqliteIndexStorage.getFirstById<StorageNode>(nodeId).serializedName);
}

std::vector<NameHierarchy> PersistentStorage::getNameHierarchiesForNodeIds(const std::vector<Id>& nodeIds) const
{
	TRACE();

	std::vector<NameHierarchy> nameHierarchies;
	for (const StorageNode& storageNode : m_sqliteIndexStorage.getAllByIds<StorageNode>(nodeIds))
	{
		nameHierarchies.push_back(NameHierarchy::deserialize(storageNode.serializedName));
	}
	return nameHierarchies;
}

NodeType PersistentStorage::getNodeTypeForNodeWithId(Id nodeId) const
{
	return utility::intToType(m_sqliteIndexStorage.getFirstById<StorageNode>(nodeId).type);
}

Id PersistentStorage::getIdForEdge(
	Edge::EdgeType type, const NameHierarchy& fromNameHierarchy, const NameHierarchy& toNameHierarchy
) const
{
	const Id sourceId = getNodeIdForNameHierarchy(fromNameHierarchy);
	const Id targetId = getNodeIdForNameHierarchy(toNameHierarchy);
	return m_sqliteIndexStorage.getEdgeBySourceTargetType(sourceId, targetId, type).id;
}

StorageEdge PersistentStorage::getEdgeById(Id edgeId) const
{
	return m_sqliteIndexStorage.getEdgeById(edgeId);
}

std::shared_ptr<SourceLocationCollection> PersistentStorage::getFullTextSearchLocations(
		const std::wstring& searchTerm, bool caseSensitive
) const
{
	TRACE();

	const TextCodec codec(ApplicationSettings::getInstance()->getTextEncoding());

	std::shared_ptr<SourceLocationCollection> collection = std::make_shared<SourceLocationCollection>();
	if (searchTerm.empty())
	{
		return collection;
	}

	if (m_fullTextSearchCodec != codec.getName())
	{
		MessageStatus(L"Building fulltext search index", false, true).dispatch();
		buildFullTextSearchIndex();
	}

	MessageStatus(
		std::wstring(L"Searching fulltext (case-") + (caseSensitive ? L"sensitive" : L"insensitive") + L"): " + searchTerm,
		false, true
	).dispatch();

	const int termLength = searchTerm.length();

	for (const FullTextSearchResult& fileHits : m_fullTextSearchIndex.searchForTerm(searchTerm))
	{
		const FilePath filePath = getFileNodePath(fileHits.fileId);
		std::shared_ptr<TextAccess> fileContent = getFileContent(filePath);

		int charsTotal = 0;
		int lineNumber = 1;
		std::wstring line = codec.decode(fileContent->getLine(lineNumber));

		for (int pos : fileHits.positions)
		{
			while (charsTotal + (int)line.length() <= pos)
			{
				charsTotal += line.length();
				lineNumber++;
				line = codec.decode(fileContent->getLine(lineNumber));
			}

			ParseLocation location;
			location.startLineNumber = lineNumber;
			location.startColumnNumber = pos - charsTotal + 1;

			if (caseSensitive && line.substr(location.startColumnNumber - 1, termLength) != searchTerm)
			{
				continue;
			}

			while ((charsTotal + (int)line.length()) < pos + termLength)
			{
				charsTotal += line.length();
				lineNumber++;
				line = codec.decode(fileContent->getLine(lineNumber));
			}

			location.endLineNumber = lineNumber;
			location.endColumnNumber = pos + termLength - charsTotal;

			// Set first bit to 1 to avoid collisions
			const Id locationId = ~(~Id(0) >> 1) + collection->getSourceLocationCount() + 1;

			collection->addSourceLocation(
				LOCATION_FULLTEXT_SEARCH,
				locationId,
				std::vector<Id>(),
				filePath,
				location.startLineNumber,
				location.startColumnNumber,
				location.endLineNumber,
				location.endColumnNumber
			);
		}
	}

	addCompleteFlagsToSourceLocationCollection(collection.get());

	MessageStatus(
		std::to_wstring(collection->getSourceLocationCount()) + L" results in " +
			std::to_wstring(collection->getSourceLocationFileCount()) + L" files for fulltext search (case-" +
			(caseSensitive ? L"sensitive" : L"insensitive") + L"): " + searchTerm,
		false, false
	).dispatch();

	return collection;
}

std::vector<SearchMatch> PersistentStorage::getAutocompletionMatches(const std::wstring& query, NodeTypeSet acceptedNodeTypes) const
{
	TRACE();

	// search in indices
	const size_t maxResultsCount = 100;
	const size_t maxBestScoredResultsLength = 100;

	// create SearchMatches
	std::vector<SearchMatch> matches;

	if (!acceptedNodeTypes.getWithMatchingRemoved([](const NodeType& type) { return type.isFile(); }).isEmpty())
	{
		utility::append(matches, getAutocompletionSymbolMatches(query, acceptedNodeTypes, maxResultsCount, maxBestScoredResultsLength));
	}

	if (acceptedNodeTypes.containsMatching([](const NodeType& type) { return type.isFile(); }))
	{
		utility::append(matches, getAutocompletionFileMatches(query, maxResultsCount));
	}

	utility::append(matches, getAutocompletionCommandMatches(query, acceptedNodeTypes));

	std::set<SearchMatch> matchesSet;
	for (SearchMatch& match : matches)
	{
		// rescore match
		if (!match.subtext.empty() && match.indices.size())
		{
			SearchResult newResult =
				SearchIndex::rescoreText(match.name, match.text, match.indices, match.score, maxBestScoredResultsLength);

			match.score = newResult.score;
			match.indices = newResult.indices;
		}

		matchesSet.insert(match);
	}

	// for (auto a : matchesSet)
	// {
	// 	std::cout << a.score << " " << a.name << std::endl;
	// }

	return utility::toVector(matchesSet);
}

std::vector<SearchMatch> PersistentStorage::getAutocompletionSymbolMatches(
	const std::wstring& query, const NodeTypeSet& acceptedNodeTypes, size_t maxResultsCount, size_t maxBestScoredResultsLength) const
{
	// search in indices
	const std::vector<SearchResult> results =
		m_symbolIndex.search(query, acceptedNodeTypes, maxResultsCount, maxBestScoredResultsLength);

	// fetch StorageNodes for node ids
	std::map<Id, StorageNode> storageNodeMap;
	std::map<Id, StorageSymbol> storageSymbolMap;
	{
		std::vector<Id> elementIds;

		for (const SearchResult& result : results)
		{
			elementIds.insert(elementIds.end(), result.elementIds.begin(), result.elementIds.end());
		}

		for (const StorageNode& node : m_sqliteIndexStorage.getAllByIds<StorageNode>(elementIds))
		{
			storageNodeMap[node.id] = node;
		}

		for (const StorageSymbol& symbol : m_sqliteIndexStorage.getAllByIds<StorageSymbol>(elementIds))
		{
			storageSymbolMap[symbol.id] = symbol;
		}
	}

	// create SearchMatches
	std::vector<SearchMatch> matches;
	for (const SearchResult& result : results)
	{
		SearchMatch match;
		const StorageNode* firstNode = nullptr;

		for (const Id& elementId : result.elementIds)
		{
			if (elementId != 0)
			{
				match.tokenIds.push_back(elementId);

				if (!match.hasChildren && acceptedNodeTypes == NodeTypeSet::all()) // TODO: check if node types of children match
				{
					match.hasChildren = m_hierarchyCache.nodeHasChildren(elementId);
				}

				if (!firstNode)
				{
					firstNode = &storageNodeMap[elementId];
				}
			}
		}

		match.name = result.text;
		match.text = result.text;

		NameHierarchy name = NameHierarchy::deserialize(firstNode->serializedName);
		if (name.getQualifiedName() == match.name)
		{
			const size_t idx = m_hierarchyCache.getIndexOfLastVisibleParentNode(firstNode->id);
			match.text = name.getRange(idx, name.size()).getQualifiedName();
			match.subtext = name.getRange(0, idx).getQualifiedName();
		}

		match.delimiter = name.getDelimiter();

		match.indices = result.indices;
		match.score = result.score;
		match.nodeType = utility::intToType(firstNode->type);
		match.typeName = match.nodeType.getReadableTypeWString();
		match.searchType = SearchMatch::SEARCH_TOKEN;

		if (storageSymbolMap.find(firstNode->id) == storageSymbolMap.end())
		{
			match.typeName = L"non-indexed " + match.typeName;
		}

		matches.push_back(match);
	}

	return matches;
}

std::vector<SearchMatch> PersistentStorage::getAutocompletionFileMatches(const std::wstring& query, size_t maxResultsCount) const
{
	const std::vector<SearchResult> results = m_fileIndex.search(
		query,
		NodeTypeSet::all().getWithMatchingKept([](const NodeType& type) { return type.isFile(); }),
		maxResultsCount,
		100
	);

	// create SearchMatches
	std::vector<SearchMatch> matches;
	for (const SearchResult& result : results)
	{
		SearchMatch match;

		match.name = result.text;
		match.tokenIds = utility::toVector(result.elementIds);

		const FilePath path(match.name);
		match.text = path.fileName();
		match.subtext = path.wstr();

		match.delimiter = NAME_DELIMITER_FILE;

		match.indices = result.indices;
		match.score = result.score;

		match.nodeType = NodeType::NODE_FILE;
		match.typeName = match.nodeType.getReadableTypeWString();

		match.searchType = SearchMatch::SEARCH_TOKEN;

		matches.push_back(match);
	}

	return matches;
}

std::vector<SearchMatch> PersistentStorage::getAutocompletionCommandMatches(
	const std::wstring& query, NodeTypeSet acceptedNodeTypes) const
{
	// search in indices
	const std::vector<SearchResult> results = m_commandIndex.search(query, NodeTypeSet::all(), 0);

	// create SearchMatches
	std::vector<SearchMatch> matches;
	for (const SearchResult& result : results)
	{
		SearchMatch match;

		match.name = result.text;
		match.text = result.text;

		match.delimiter = NAME_DELIMITER_UNKNOWN;

		match.indices = result.indices;
		match.score = result.score;

		match.searchType = SearchMatch::SEARCH_COMMAND;
		match.typeName = L"command";

		if (match.getCommandType() == SearchMatch::COMMAND_NODE_FILTER)
		{
			match.nodeType = utility::getTypeForReadableTypeString(match.name);
			match.typeName = L"filter";
		}

		if (acceptedNodeTypes == NodeTypeSet::all() ||
			(match.getCommandType() == SearchMatch::COMMAND_NODE_FILTER && !(acceptedNodeTypes.contains(match.nodeType))))
		{
			matches.push_back(match);
		}
	}

	return matches;
}

std::vector<SearchMatch> PersistentStorage::getSearchMatchesForTokenIds(const std::vector<Id>& elementIds) const
{
	TRACE();

	// todo: what if all these elements share the same node in the searchindex?
	// In that case there should be only one search match.
	std::vector<SearchMatch> matches;

	// fetch StorageNodes for node ids
	std::map<Id, StorageNode> storageNodeMap;
	for (StorageNode& node : m_sqliteIndexStorage.getAllByIds<StorageNode>(elementIds))
	{
		storageNodeMap.emplace(node.id, node);
	}

	for (Id elementId : elementIds)
	{
		if (storageNodeMap.find(elementId) == storageNodeMap.end())
		{
			continue;
		}

		StorageNode node = storageNodeMap[elementId];

		SearchMatch match;
		const NameHierarchy nameHierarchy = NameHierarchy::deserialize(node.serializedName);
		match.name = nameHierarchy.getQualifiedName();
		match.text = nameHierarchy.getRawName();

		match.tokenIds.push_back(elementId);
		match.nodeType = utility::intToType(node.type);
		match.searchType = SearchMatch::SEARCH_TOKEN;

		match.delimiter = nameHierarchy.getDelimiter();

		if (match.nodeType.isFile())
		{
			match.text = FilePath(match.text).fileName();
		}

		matches.push_back(match);
	}

	return matches;
}

std::shared_ptr<Graph> PersistentStorage::getGraphForAll() const
{
	TRACE();

	std::shared_ptr<Graph> graph = std::make_shared<Graph>();

	std::vector<Id> tokenIds;
	for (StorageNode& node: m_sqliteIndexStorage.getAll<StorageNode>())
	{
		auto it = m_symbolDefinitionKinds.find(node.id);
		if (it != m_symbolDefinitionKinds.end() && it->second == DEFINITION_EXPLICIT &&
			(
				NodeType(utility::intToType(node.type)).isPackage() ||
				!m_hierarchyCache.isChildOfVisibleNodeOrInvisible(node.id)
			)
		){
			tokenIds.push_back(node.id);
		}
	}

	for (const auto& p : m_fileNodePaths)
	{
		tokenIds.push_back(p.first);
	}

	addNodesToGraph(tokenIds, graph.get(), false);

	return graph;
}

std::shared_ptr<Graph> PersistentStorage::getGraphForNodeTypes(NodeTypeSet nodeTypes) const
{
	TRACE();

	std::shared_ptr<Graph> graph = std::make_shared<Graph>();

	std::vector<Id> tokenIds;
	for (StorageNode& node: m_sqliteIndexStorage.getAll<StorageNode>())
	{
		if (nodeTypes.contains(utility::intToType(node.type)))
		{
			auto it = m_symbolDefinitionKinds.find(node.id);
			if (it != m_symbolDefinitionKinds.end() && it->second == DEFINITION_EXPLICIT)
			{
				tokenIds.push_back(node.id);
			}
		}
	}

	if (nodeTypes.containsMatching([](const NodeType& type) { return type.isFile(); }))
	{
		for (const auto& p : m_fileNodePaths)
		{
			tokenIds.push_back(p.first);
		}
	}

	addNodesWithParentsAndEdgesToGraph(tokenIds, std::vector<Id>(), graph.get(), false);

	return graph;
}

std::shared_ptr<Graph> PersistentStorage::getGraphForActiveTokenIds(
	const std::vector<Id>& tokenIds, const std::vector<Id>& expandedNodeIds, bool* isActiveNamespace) const
{
	TRACE();

	std::vector<Id> ids(tokenIds);
	bool isPackage = false;

	std::vector<Id> nodeIds;
	std::vector<Id> edgeIds;

	bool addAggregations = false;
	std::vector<StorageEdge> edgesToAggregate;

	bool addFileContents = false;

	if (tokenIds.size() == 1)
	{
		const Id elementId = tokenIds[0];
		const StorageNode node = m_sqliteIndexStorage.getFirstById<StorageNode>(elementId);

		if (node.id > 0)
		{
			const NodeType nodeType = utility::intToType(node.type);
			if (nodeType.isPackage())
			{
				ids.clear();
				m_hierarchyCache.addFirstChildIdsForNodeId(elementId, &ids, &edgeIds);
				edgeIds.clear();

				isPackage = true;
			}
			else
			{
				m_hierarchyCache.addFirstChildIdsForNodeId(elementId, &nodeIds, &edgeIds);

				// don't expand active node if it has more than 20 child nodes
				if (nodeIds.size() > 20 && nodeType.isCollapsible())
				{
					nodeIds.clear();
				}

				nodeIds.push_back(elementId);
				edgeIds.clear();

				for (const StorageEdge& edge : m_sqliteIndexStorage.getEdgesBySourceOrTargetId(elementId))
				{
					Edge::EdgeType edgeType = Edge::intToType(edge.type);
					if (edgeType == Edge::EDGE_MEMBER)
					{
						continue;
					}

					if (nodeType.isUsable() && (edgeType & Edge::EDGE_TYPE_USAGE) &&
						m_hierarchyCache.isChildOfVisibleNodeOrInvisible(edge.sourceNodeId) &&
						(m_hierarchyCache.getLastVisibleParentNodeId(edge.targetNodeId) !=
							m_hierarchyCache.getLastVisibleParentNodeId(edge.sourceNodeId)))
					{
						edgesToAggregate.push_back(edge);
					}
					else
					{
						edgeIds.push_back(edge.id);
					}
				}

				if (nodeType.isFile())
				{
					addFileContents = true;
				}
				else
				{
					addAggregations = true;
				}
			}
		}
		else if (m_sqliteIndexStorage.isEdge(elementId))
		{
			edgeIds.push_back(elementId);
		}
	}

	if (ids.size() >= 1 || isPackage)
	{
		std::set<Id> symbolIds;
		for (const StorageSymbol& symbol : m_sqliteIndexStorage.getAllByIds<StorageSymbol>(ids))
		{
			if (symbol.id > 0 && (!isPackage || intToDefinitionKind(symbol.definitionKind) != DEFINITION_IMPLICIT))
			{
				nodeIds.push_back(symbol.id);
			}
			symbolIds.insert(symbol.id);
		}
		for (const StorageNode& node : m_sqliteIndexStorage.getAllByIds<StorageNode>(ids))
		{
			if (symbolIds.find(node.id) == symbolIds.end())
			{
				nodeIds.push_back(node.id);
			}
		}

		if (!isPackage)
		{
			if (nodeIds.size() != ids.size())
			{
				for (const StorageEdge& edge : m_sqliteIndexStorage.getAllByIds<StorageEdge>(ids))
				{
					if (edge.id > 0)
					{
						edgeIds.push_back(edge.id);
					}
				}
			}
		}
	}

	std::shared_ptr<Graph> g = std::make_shared<Graph>();
	Graph* graph = g.get();

	if (isPackage)
	{
		addNodesToGraph(nodeIds, graph, false);
	}
	else
	{
		addNodesWithParentsAndEdgesToGraph(nodeIds, edgeIds, graph, true);
	}

	if (addAggregations)
	{
		addAggregationEdgesToGraph(tokenIds[0], edgesToAggregate, graph);
	}
	else if (addFileContents)
	{
		addFileContentsToGraph(tokenIds[0], graph);
	}

	if (!isPackage)
	{
		std::vector<Id> expandedChildIds;
		std::vector<Id> expandedChildEdgeIds;

		for (Id nodeId : expandedNodeIds)
		{
			if (graph->getNodeById(nodeId))
			{
				m_hierarchyCache.addFirstChildIdsForNodeId(nodeId, &expandedChildIds, &expandedChildEdgeIds);
			}
		}

		if (expandedChildIds.size())
		{
			addNodesToGraph(expandedChildIds, graph, true);
			addEdgesToGraph(expandedChildEdgeIds, graph);
		}

		addInheritanceChainsToGraph(nodeIds, graph);
	}

	addComponentAccessToGraph(graph);

	if (isActiveNamespace)
	{
		*isActiveNamespace = isPackage;
	}

	return g;
}

std::shared_ptr<Graph> PersistentStorage::getGraphForChildrenOfNodeId(Id nodeId) const
{
	TRACE();

	std::vector<Id> nodeIds;
	std::vector<Id> edgeIds;

	nodeIds.push_back(nodeId);
	m_hierarchyCache.addFirstChildIdsForNodeId(nodeId, &nodeIds, &edgeIds);

	std::shared_ptr<Graph> graph = std::make_shared<Graph>();
	addNodesToGraph(nodeIds, graph.get(), true);
	addEdgesToGraph(edgeIds, graph.get());

	addComponentAccessToGraph(graph.get());
	return graph;
}

std::shared_ptr<Graph> PersistentStorage::getGraphForTrail(
	Id originId, Id targetId, Edge::TypeMask trailType, size_t depth) const
{
	TRACE();

	std::set<Id> nodeIds;
	std::set<Id> edgeIds;

	std::vector<Id> nodeIdsToProcess;
	nodeIdsToProcess.push_back(originId ? originId : targetId);
	bool forward = originId;
	size_t currentDepth = 0;

	nodeIds.insert(nodeIdsToProcess.back());
	while (nodeIdsToProcess.size() && (!depth || currentDepth < depth))
	{
		std::vector<StorageEdge> edges =
			forward ?
			m_sqliteIndexStorage.getEdgesBySourceIds(nodeIdsToProcess) :
			m_sqliteIndexStorage.getEdgesByTargetIds(nodeIdsToProcess);

		if (trailType & Edge::LAYOUT_VERTICAL)
		{
			utility::append(edges,
				forward ?
				m_sqliteIndexStorage.getEdgesByTargetIds(nodeIdsToProcess) :
				m_sqliteIndexStorage.getEdgesBySourceIds(nodeIdsToProcess)
			);
		}

		nodeIdsToProcess.clear();

		for (const StorageEdge& edge : edges)
		{
			if (Edge::intToType(edge.type) & trailType)
			{
				bool isForward = forward == !(Edge::intToType(edge.type) & Edge::LAYOUT_VERTICAL);

				const Id nodeId = isForward ? edge.targetNodeId : edge.sourceNodeId;
				const Id otherNodeId = isForward ? edge.sourceNodeId : edge.targetNodeId;

				if (nodeIds.find(nodeId) == nodeIds.end())
				{
					nodeIdsToProcess.push_back(nodeId);
					nodeIds.insert(nodeId);
					edgeIds.insert(edge.id);
				}
				else if (nodeIds.find(otherNodeId) != nodeIds.end())
				{
					edgeIds.insert(edge.id);
				}
			}
		}

		currentDepth++;
	}

	std::shared_ptr<Graph> graph = std::make_shared<Graph>();

	addNodesWithParentsAndEdgesToGraph(utility::toVector(nodeIds), utility::toVector(edgeIds), graph.get(), false);
	addComponentAccessToGraph(graph.get());

	return graph;
}

// TODO: rename: getActiveElementIdsForId; TODO: make separate function for declarationId
std::vector<Id> PersistentStorage::getActiveTokenIdsForId(Id tokenId, Id* declarationId) const
{
	TRACE();

	std::vector<Id> activeTokenIds;

	bool isNode = m_sqliteIndexStorage.isNode(tokenId);
	bool isEdge = m_sqliteIndexStorage.isEdge(tokenId);

	if (!isEdge && !isNode)
	{
		return activeTokenIds;
	}

	activeTokenIds.push_back(tokenId);

	if (isNode)
	{
		*declarationId = tokenId;

		for (const StorageEdge& edge : m_sqliteIndexStorage.getEdgesByTargetId(tokenId))
		{
			activeTokenIds.push_back(edge.id);
		}
	}

	return activeTokenIds;
}

std::vector<Id> PersistentStorage::getNodeIdsForLocationIds(const std::vector<Id>& locationIds) const
{
	TRACE();

	std::set<Id> edgeIds;
	std::set<Id> nodeIds;
	std::set<Id> implicitEdgeIds;
	std::set<Id> implicitNodeIds;

	for (const StorageOccurrence& occurrence: m_sqliteIndexStorage.getOccurrencesForLocationIds(locationIds))
	{
		const Id elementId = occurrence.elementId;

		const StorageEdge edge = m_sqliteIndexStorage.getFirstById<StorageEdge>(elementId);
		if (edge.id != 0)
		{
			auto it = m_symbolDefinitionKinds.find(edge.targetNodeId);
			if (it != m_symbolDefinitionKinds.end() && it->second == DEFINITION_IMPLICIT)
			{
				implicitEdgeIds.insert(edge.targetNodeId);
			}
			else
			{
				edgeIds.insert(edge.targetNodeId);
			}
		}
		else if (m_sqliteIndexStorage.isNode(elementId))
		{
			auto it = m_symbolDefinitionKinds.find(elementId);
			if (it != m_symbolDefinitionKinds.end() && it->second == DEFINITION_IMPLICIT)
			{
				implicitNodeIds.insert(elementId);
			}
			else
			{
				nodeIds.insert(elementId);
			}
		}
	}

	if (nodeIds.size())
	{
		return utility::toVector(nodeIds);
	}
	else if (implicitNodeIds.size())
	{
		return utility::toVector(implicitNodeIds);
	}
	else if (edgeIds.size())
	{
		return utility::toVector(edgeIds);
	}
	else
	{
		return utility::toVector(implicitEdgeIds);
	}
}

std::shared_ptr<SourceLocationCollection> PersistentStorage::getSourceLocationsForTokenIds(
	const std::vector<Id>& tokenIds) const
{
	TRACE();

	std::vector<FilePath> filePaths;
	std::vector<Id> nonFileIds;

	for (const Id tokenId : tokenIds)
	{
		FilePath path = getFileNodePath(tokenId);

		// check for non-indexed file
		if (path.empty() && m_symbolDefinitionKinds.find(tokenId) == m_symbolDefinitionKinds.end())
		{
			const StorageNode fileNode = m_sqliteIndexStorage.getNodeById(tokenId);
			if (NodeType(utility::intToType(fileNode.type)).isFile())
			{
				path = FilePath(NameHierarchy::deserialize(fileNode.serializedName).getQualifiedName());
			}
		}

		if (path.empty())
		{
			nonFileIds.push_back(tokenId);
		}
		else
		{
			filePaths.push_back(path);
		}
	}

	std::shared_ptr<SourceLocationCollection> collection = std::make_shared<SourceLocationCollection>();
	for (const FilePath& path : filePaths)
	{
		collection->addSourceLocationFile(std::make_shared<SourceLocationFile>(path, true, false));
	}

	if (nonFileIds.size())
	{
		std::vector<Id> locationIds;
		std::unordered_map<Id, Id> locationIdToElementIdMap;
		for (const StorageOccurrence& occurrence: m_sqliteIndexStorage.getOccurrencesForElementIds(nonFileIds))
		{
			locationIds.push_back(occurrence.sourceLocationId);
			locationIdToElementIdMap[occurrence.sourceLocationId] = occurrence.elementId;
		}

		for (const StorageSourceLocation& sourceLocation: m_sqliteIndexStorage.getAllByIds<StorageSourceLocation>(locationIds))
		{
			const LocationType type = intToLocationType(sourceLocation.type);
			if (type == LOCATION_QUALIFIER)
			{
				continue;
			}

			auto it = locationIdToElementIdMap.find(sourceLocation.id);
			if (it == locationIdToElementIdMap.end())
			{
				continue;
			}

			FilePath path = getFileNodePath(sourceLocation.fileNodeId);
			if (path.empty())
			{
				const StorageNode fileNode = m_sqliteIndexStorage.getNodeById(sourceLocation.fileNodeId);
				if (fileNode.id)
				{
					const FilePath path2 = FilePath(NameHierarchy::deserialize(fileNode.serializedName).getQualifiedName());
					if (path2.exists())
					{
						path = path2;
					}
				}
			}

			if (!path.empty())
			{
				collection->addSourceLocation(
					type,
					sourceLocation.id,
					std::vector<Id>(1, it->second),
					path,
					sourceLocation.startLine,
					sourceLocation.startCol,
					sourceLocation.endLine,
					sourceLocation.endCol
				);
			}
		}
	}

	addCompleteFlagsToSourceLocationCollection(collection.get());

	return collection;
}

std::shared_ptr<SourceLocationCollection> PersistentStorage::getSourceLocationsForLocationIds(
		const std::vector<Id>& locationIds
) const
{
	TRACE();

	std::shared_ptr<SourceLocationCollection> collection = std::make_shared<SourceLocationCollection>();

	for (StorageSourceLocation location: m_sqliteIndexStorage.getAllByIds<StorageSourceLocation>(locationIds))
	{
		std::vector<Id> elementIds;
		for (const StorageOccurrence& occurrence: m_sqliteIndexStorage.getOccurrencesForLocationId(location.id))
		{
			elementIds.push_back(occurrence.elementId);
		}

		collection->addSourceLocation(
			intToLocationType(location.type),
			location.id,
			elementIds,
			getFileNodePath(location.fileNodeId),
			location.startLine,
			location.startCol,
			location.endLine,
			location.endCol
		);
	}

	addCompleteFlagsToSourceLocationCollection(collection.get());

	return collection;
}

std::shared_ptr<SourceLocationFile> PersistentStorage::getSourceLocationsForFile(const FilePath& filePath) const
{
	TRACE();

	return m_sqliteIndexStorage.getSourceLocationsForFile(filePath);
}

std::shared_ptr<SourceLocationFile> PersistentStorage::getSourceLocationsForLinesInFile(
		const FilePath& filePath, size_t startLine, size_t endLine
) const
{
	TRACE();

	return m_sqliteIndexStorage.getSourceLocationsForLinesInFile(
		filePath, startLine, endLine)->getFilteredByLines(startLine, endLine);
}

std::shared_ptr<SourceLocationFile> PersistentStorage::getSourceLocationsOfTypeInFile(
		const FilePath& filePath, LocationType type
) const
{
	TRACE();

	return m_sqliteIndexStorage.getSourceLocationsOfTypeInFile(filePath, type);
}

std::shared_ptr<SourceLocationFile> PersistentStorage::getCommentLocationsInFile(const FilePath& filePath) const
{
	TRACE();

	const std::shared_ptr<SourceLocationFile> file = std::make_shared<SourceLocationFile>(filePath, false, false);

	const std::vector<StorageCommentLocation> storageLocations = m_sqliteIndexStorage.getCommentLocationsInFile(filePath);
	for (size_t i = 0; i < storageLocations.size(); i++)
	{
		file->addSourceLocation(
			LOCATION_TOKEN,
			storageLocations[i].id,
			std::vector<Id>(), // comment token location has no element.
			storageLocations[i].startLine,
			storageLocations[i].startCol,
			storageLocations[i].endLine,
			storageLocations[i].endCol
		);
	}

	return file;
}

std::shared_ptr<TextAccess> PersistentStorage::getFileContent(const FilePath& filePath) const
{
	TRACE();

	return m_sqliteIndexStorage.getFileContentByPath(filePath.wstr());
}

FileInfo PersistentStorage::getFileInfoForFilePath(const FilePath& filePath) const
{
	return FileInfo(filePath, m_sqliteIndexStorage.getFileByPath(filePath.wstr()).modificationTime);
}

std::vector<FileInfo> PersistentStorage::getFileInfosForFilePaths(const std::vector<FilePath>& filePaths) const
{
	std::vector<FileInfo> fileInfos;

	for (const StorageFile& file : m_sqliteIndexStorage.getFilesByPaths(filePaths))
	{
		fileInfos.push_back(FileInfo(FilePath(file.filePath), file.modificationTime));
	}

	return fileInfos;
}

StorageStats PersistentStorage::getStorageStats() const
{
	TRACE();

	StorageStats stats;

	stats.nodeCount = m_sqliteIndexStorage.getNodeCount();
	stats.edgeCount = m_sqliteIndexStorage.getEdgeCount();

	stats.fileCount = m_sqliteIndexStorage.getFileCount();
	stats.completedFileCount = m_sqliteIndexStorage.getCompletedFileCount();
	stats.fileLOCCount = m_sqliteIndexStorage.getFileLineSum();

	stats.timestamp = m_sqliteIndexStorage.getTime();

	return stats;
}

ErrorCountInfo PersistentStorage::getErrorCount() const
{
	return getErrorCount(getErrors());
}

ErrorCountInfo PersistentStorage::getErrorCount(const std::vector<ErrorInfo>& errors) const
{
	ErrorCountInfo info;

	for (const ErrorInfo& error : errors)
	{
		info.total++;

		if (error.fatal)
		{
			info.fatal++;
		}
	}

	return info;
}

std::vector<ErrorInfo> PersistentStorage::getErrors() const
{
	std::vector<ErrorInfo> errors;

	for (const ErrorInfo& error : m_sqliteIndexStorage.getAll<StorageError>())
	{
		if (m_errorFilter.filter(error))
		{
			errors.push_back(error);
		}
	}

	return errors;
}

std::vector<ErrorInfo> PersistentStorage::getErrorsLimited() const
{
	std::vector<ErrorInfo> errors;

	for (const ErrorInfo& error : m_sqliteIndexStorage.getAll<StorageError>())
	{
		if (m_errorFilter.filter(error))
		{
			errors.push_back(error);

			if (m_errorFilter.limit > 0 && errors.size() >= m_errorFilter.limit)
			{
				break;
			}
		}
	}

	return errors;
}

std::vector<ErrorInfo> PersistentStorage::getErrorsForFileLimited(const FilePath& filePath) const
{
	std::unordered_map<Id, std::set<Id>> includingMap = getFileIdToIncludedFileIdMap();

	std::set<FilePath> filePaths;
	filePaths.insert(filePath);

	std::set<Id> fileIdsToProcess = includingMap[getFileNodeId(filePath)];
	std::set<Id> processedFileIds;
	while (fileIdsToProcess.size())
	{
		std::set<Id> nextFileIdsToProcess;
		for (Id id : fileIdsToProcess)
		{
			if (filePaths.insert(getFileNodePath(id)).second)
			{
				utility::append(nextFileIdsToProcess, includingMap[id]);
			}
		}
		fileIdsToProcess = nextFileIdsToProcess;
	}

	std::vector<ErrorInfo> errors;

	for (const ErrorInfo& error : m_sqliteIndexStorage.getAll<StorageError>())
	{
		if (m_errorFilter.filter(error) && filePaths.find(FilePath(error.filePath)) != filePaths.end())
		{
			errors.push_back(error);

			if (m_errorFilter.limit > 0 && errors.size() >= m_errorFilter.limit)
			{
				break;
			}
		}
	}

	return errors;
}

std::shared_ptr<SourceLocationCollection> PersistentStorage::getErrorSourceLocations(
	const std::vector<ErrorInfo>& errors) const
{
	TRACE();

	std::shared_ptr<SourceLocationCollection> collection = std::make_shared<SourceLocationCollection>();
	size_t count = 0;
	for (const ErrorInfo& error : errors)
	{
		// Set first bit to 1 to avoid collisions
		Id locationId = ~(~Id(0) >> 1) + error.id;

		collection->addSourceLocation(
			LOCATION_ERROR,
			locationId,
			std::vector<Id>(1, error.id),
			FilePath(error.filePath),
			error.lineNumber,
			error.columnNumber,
			error.lineNumber,
			error.columnNumber
		);

		count++;

		if (m_errorFilter.limit > 0 && count >= m_errorFilter.limit)
		{
			break;
		}
	}

	addCompleteFlagsToSourceLocationCollection(collection.get());

	return collection;
}

Id PersistentStorage::addNodeBookmark(const NodeBookmark& bookmark)
{
	const Id categoryId = addBookmarkCategory(bookmark.getCategory().getName());
	const Id id = m_sqliteBookmarkStorage.addBookmark(StorageBookmarkData(
		bookmark.getName(), bookmark.getComment(), bookmark.getTimeStamp().toString(), categoryId
	)).id;

	for (const Id& nodeId: bookmark.getNodeIds())
	{
		m_sqliteBookmarkStorage.addBookmarkedNode(StorageBookmarkedNodeData(id, m_sqliteIndexStorage.getNodeById(nodeId).serializedName));
	}

	return id;
}

Id PersistentStorage::addEdgeBookmark(const EdgeBookmark& bookmark)
{
	const Id categoryId = addBookmarkCategory(bookmark.getCategory().getName());
	const Id id = m_sqliteBookmarkStorage.addBookmark(StorageBookmarkData(
		bookmark.getName(), bookmark.getComment(), bookmark.getTimeStamp().toString(), categoryId
	)).id;
	for (const Id& edgeId: bookmark.getEdgeIds())
	{
		const StorageEdge storageEdge = m_sqliteIndexStorage.getEdgeById(edgeId);

		bool sourceNodeActive = storageEdge.sourceNodeId == bookmark.getActiveNodeId();
		m_sqliteBookmarkStorage.addBookmarkedEdge(StorageBookmarkedEdgeData(
			id,
			// todo: optimization for multiple edges in same bookmark: use a local cache here
			m_sqliteIndexStorage.getNodeById(storageEdge.sourceNodeId).serializedName,
			m_sqliteIndexStorage.getNodeById(storageEdge.targetNodeId).serializedName,
			storageEdge.type,
			sourceNodeActive
		));
	}
	return id;
}

Id PersistentStorage::addBookmarkCategory(const std::wstring& name)
{
	if (name.empty())
	{
		return 0;
	}

	Id id = m_sqliteBookmarkStorage.getBookmarkCategoryByName(name).id;
	if (id == 0)
	{
		id = m_sqliteBookmarkStorage.addBookmarkCategory(StorageBookmarkCategoryData(name)).id;
	}
	return id;
}

void PersistentStorage::updateBookmark(
	const Id bookmarkId, const std::wstring& name, const std::wstring& comment, const std::wstring& categoryName)
{
	const Id categoryId = addBookmarkCategory(categoryName); // only creates category if id didn't exist before;
	m_sqliteBookmarkStorage.updateBookmark(bookmarkId, name, comment, categoryId);
}

void PersistentStorage::removeBookmark(const Id id)
{
	m_sqliteBookmarkStorage.removeBookmark(id);
}

void PersistentStorage::removeBookmarkCategory(Id id)
{
	m_sqliteBookmarkStorage.removeBookmarkCategory(id);
}

std::vector<NodeBookmark> PersistentStorage::getAllNodeBookmarks() const
{
	std::unordered_map<Id, StorageBookmarkCategory> bookmarkCategories;
	for (const StorageBookmarkCategory& bookmarkCategory: m_sqliteBookmarkStorage.getAllBookmarkCategories())
	{
		bookmarkCategories[bookmarkCategory.id] = bookmarkCategory;
	}

	std::unordered_map<Id, std::vector<Id>> bookmarkIdToBookmarkedNodeIds;
	for (const StorageBookmarkedNode& bookmarkedNode: m_sqliteBookmarkStorage.getAllBookmarkedNodes())
	{
		bookmarkIdToBookmarkedNodeIds[bookmarkedNode.bookmarkId].push_back(
			m_sqliteIndexStorage.getNodeBySerializedName(bookmarkedNode.serializedNodeName).id);
	}

	std::vector<NodeBookmark> nodeBookmarks;

	for (const StorageBookmark& storageBookmark: m_sqliteBookmarkStorage.getAllBookmarks())
	{
		auto itCategories = bookmarkCategories.find(storageBookmark.categoryId);
		auto itNodeIds = bookmarkIdToBookmarkedNodeIds.find(storageBookmark.id);
		if (itCategories != bookmarkCategories.end() && itNodeIds != bookmarkIdToBookmarkedNodeIds.end())
		{
			NodeBookmark bookmark(
				storageBookmark.id,
				storageBookmark.name,
				storageBookmark.comment,
				storageBookmark.timestamp,
				BookmarkCategory(itCategories->second.id, itCategories->second.name)
			);
			bookmark.setNodeIds(itNodeIds->second);
			bookmark.setIsValid();
			nodeBookmarks.push_back(bookmark);
		}
	}

	return nodeBookmarks;
}

std::vector<EdgeBookmark> PersistentStorage::getAllEdgeBookmarks() const
{
	std::unordered_map<Id, StorageBookmarkCategory> bookmarkCategories;
	for (const StorageBookmarkCategory& bookmarkCategory: m_sqliteBookmarkStorage.getAllBookmarkCategories())
	{
		bookmarkCategories[bookmarkCategory.id] = bookmarkCategory;
	}

	std::unordered_map<Id, std::vector<StorageBookmarkedEdge>> bookmarkIdToBookmarkedEdges;
	for (const StorageBookmarkedEdge& bookmarkedEdge: m_sqliteBookmarkStorage.getAllBookmarkedEdges())
	{
		bookmarkIdToBookmarkedEdges[bookmarkedEdge.bookmarkId].push_back(bookmarkedEdge);
	}

	std::vector<EdgeBookmark> edgeBookmarks;

	UnorderedCache<std::wstring, Id> nodeIdCache(
		[&](const std::wstring& serializedNodeName)
		{
			return m_sqliteIndexStorage.getNodeBySerializedName(serializedNodeName).id;
		}
	);

	for (const StorageBookmark& storageBookmark: m_sqliteBookmarkStorage.getAllBookmarks())
	{
		auto itCategories = bookmarkCategories.find(storageBookmark.categoryId);
		auto itBookmarkedEdges = bookmarkIdToBookmarkedEdges.find(storageBookmark.id);
		if (itCategories != bookmarkCategories.end() && itBookmarkedEdges != bookmarkIdToBookmarkedEdges.end())
		{
			EdgeBookmark bookmark(
				storageBookmark.id,
				storageBookmark.name,
				storageBookmark.comment,
				storageBookmark.timestamp,
				BookmarkCategory(itCategories->second.id, itCategories->second.name)
			);

			Id activeNodeId = 0;
			for (const StorageBookmarkedEdge& bookmarkedEdge: itBookmarkedEdges->second)
			{
				const Id sourceNodeId = nodeIdCache.getValue(bookmarkedEdge.serializedSourceNodeName);
				const Id targetNodeId = nodeIdCache.getValue(bookmarkedEdge.serializedTargetNodeName);
				const Id edgeId =
					m_sqliteIndexStorage.getEdgeBySourceTargetType(sourceNodeId, targetNodeId, bookmarkedEdge.edgeType).id;
				bookmark.addEdgeId(edgeId);

				if (activeNodeId == 0)
				{
					activeNodeId = bookmarkedEdge.sourceNodeActive ? sourceNodeId : targetNodeId;
				}
			}
			bookmark.setActiveNodeId(activeNodeId);
			bookmark.setIsValid();
			edgeBookmarks.push_back(bookmark);
		}
	}

	return edgeBookmarks;
}

std::vector<BookmarkCategory> PersistentStorage::getAllBookmarkCategories() const
{
	std::vector<BookmarkCategory> categories;
	for (const StorageBookmarkCategory& storageBookmarkCategoriy : m_sqliteBookmarkStorage.getAllBookmarkCategories())
	{
		categories.push_back(BookmarkCategory(storageBookmarkCategoriy.id, storageBookmarkCategoriy.name));
	}
	return categories;
}

TooltipInfo PersistentStorage::getTooltipInfoForTokenIds(const std::vector<Id>& tokenIds, TooltipOrigin origin) const
{
	TRACE();

	TooltipInfo info;

	if (!tokenIds.size())
	{
		return info;
	}

	StorageNode node = m_sqliteIndexStorage.getFirstById<StorageNode>(tokenIds[0]);
	if (node.id == 0 && origin == TOOLTIP_ORIGIN_CODE)
	{
		const StorageEdge edge = m_sqliteIndexStorage.getFirstById<StorageEdge>(tokenIds[0]);

		if (edge.id > 0)
		{
			node = m_sqliteIndexStorage.getFirstById<StorageNode>(edge.targetNodeId);
		}
	}

	if (node.id == 0)
	{
		return info;
	}

	const NodeType type = utility::intToType(node.type);
	info.title = type.getReadableTypeWString();

	DefinitionKind defKind = DEFINITION_NONE;
	const StorageSymbol symbol = m_sqliteIndexStorage.getFirstById<StorageSymbol>(node.id);
	if (symbol.id > 0)
	{
		defKind = intToDefinitionKind(symbol.definitionKind);
	}

	if (type.isPotentialMember())
	{
		const StorageComponentAccess access = m_sqliteIndexStorage.getComponentAccessByNodeId(node.id);
		if (access.nodeId != 0)
		{
			info.title = accessKindToString(intToAccessKind(access.type)) + L" " + info.title;
		}
	}

	if (type.isFile() && m_fileNodePaths.find(node.id) != m_fileNodePaths.end())
	{
		if (!getFileNodeComplete(node.id))
		{
			info.title = L"incomplete " + info.title;
		}
	}
	else if (defKind == DEFINITION_NONE)
	{
		info.title = L"non-indexed " + info.title;
	}
	else if (defKind == DEFINITION_IMPLICIT)
	{
		info.title = L"implicit " + info.title;
	}

	info.count = 0;
	info.countText = "reference";
	for (const auto& edge : m_sqliteIndexStorage.getEdgesByTargetId(node.id))
	{
		if (Edge::intToType(edge.type) != Edge::EDGE_MEMBER)
		{
			info.count++;
		}
	}

	info.snippets.push_back(getTooltipSnippetForNode(node));

	if (origin == TOOLTIP_ORIGIN_CODE)
	{
		info.offset = Vec2i(20, 30);
	}
	else
	{
		info.offset = Vec2i(50, 20);
	}

	return info;
}

TooltipSnippet PersistentStorage::getTooltipSnippetForNode(const StorageNode& node) const
{
	TRACE();

	const NameHierarchy nameHierarchy = NameHierarchy::deserialize(node.serializedName);
	TooltipSnippet snippet;
	snippet.code = nameHierarchy.getQualifiedNameWithSignature();
	snippet.locationFile = std::make_shared<SourceLocationFile>(
		FilePath(nameHierarchy.getDelimiter() == NAME_DELIMITER_JAVA ? L"main.java" : L"main.cpp"), true, true);

	if (nameHierarchy.hasSignature())
	{
		snippet.code = utility::breakSignature(
			nameHierarchy.getSignature().getPrefix(),
			nameHierarchy.getQualifiedName(),
			nameHierarchy.getSignature().getPostfix(),
			50,
			ApplicationSettings::getInstance()->getCodeTabWidth()
		);

		std::vector<Id> typeNodeIds;
		for (const auto& edge : m_sqliteIndexStorage.getEdgesBySourceId(node.id))
		{
			if (Edge::intToType(edge.type) == Edge::EDGE_TYPE_USAGE)
			{
				typeNodeIds.push_back(edge.targetNodeId);
			}
		}

		std::set<std::pair<std::wstring, Id>, bool(*)(const std::pair<std::wstring, Id>&, const std::pair<std::wstring, Id>&)> typeNames(
			[](const std::pair<std::wstring, Id>& a, const std::pair<std::wstring, Id>& b)
			{
				if (a.first.size() == b.first.size())
				{
					return a.first < b.first;
				}

				return a.first.size() > b.first.size();
			}
		);

		typeNames.insert(std::make_pair(nameHierarchy.getQualifiedName(), node.id));
		for (const auto& typeNode : m_sqliteIndexStorage.getAllByIds<StorageNode>(typeNodeIds))
		{
			typeNames.insert(std::make_pair(
				NameHierarchy::deserialize(typeNode.serializedName).getQualifiedName(),
				typeNode.id
			));
		}

		std::vector<std::pair<size_t, size_t>> locationRanges;
		for (const auto& p : typeNames)
		{
			size_t pos = 0;
			while (pos != std::wstring::npos)
			{
				pos = snippet.code.find(p.first, pos);
				if (pos == std::wstring::npos)
				{
					continue;
				}

				bool inRange = false;
				for (const auto& locationRange : locationRanges)
				{
					if (pos + 1 >= locationRange.first && pos + 1 <= locationRange.second)
					{
						inRange = true;
						pos = locationRange.second + 1;
						break;
					}
				}

				if (!inRange)
				{
					snippet.locationFile->addSourceLocation(
						LOCATION_TOKEN, 0, std::vector<Id>(1, p.second), 1, pos + 1, 1, pos + p.first.size());

					locationRanges.push_back(std::make_pair(pos + 1, pos + p.first.size()));
					pos += p.first.size();
				}
			}
		}
	}
	else
	{
		snippet.locationFile->addSourceLocation(
			LOCATION_TOKEN, 0, std::vector<Id>(1, node.id), 1, 1, 1, snippet.code.size());
	}

	return snippet;
}

TooltipInfo PersistentStorage::getTooltipInfoForSourceLocationIdsAndLocalSymbolIds(
	const std::vector<Id>& locationIds, const std::vector<Id>& localSymbolIds) const
{
	TRACE();

	const TextCodec codec(ApplicationSettings::getInstance()->getTextEncoding());

	TooltipInfo info;

	if (locationIds.empty() && localSymbolIds.empty())
	{
		return info;
	}

	if (!locationIds.empty())
	{
		const std::vector<Id> nodeIds = getNodeIdsForLocationIds(locationIds);

		for (const StorageNode& node : m_sqliteIndexStorage.getAllByIds<StorageNode>(nodeIds))
		{
			TooltipSnippet snippet;

			const NameHierarchy nameHierarchy = NameHierarchy::deserialize(node.serializedName);
			snippet.code = nameHierarchy.getQualifiedName();
			snippet.locationFile = std::make_shared<SourceLocationFile>(
				FilePath(nameHierarchy.getDelimiter() == NAME_DELIMITER_JAVA ? L"main.java" : L"main.cpp"), true, true);

			snippet.locationFile->addSourceLocation(
				LOCATION_TOKEN, 0, std::vector<Id>(1, node.id), 1, 1, 1, snippet.code.size());

			if (NodeType(utility::intToType(node.type)).isCallable())
			{
				snippet.code += L"()";
			}

			info.snippets.push_back(snippet);
		}
	}

	for (Id id : localSymbolIds)
	{
		TooltipSnippet snippet;

		snippet.code = L"local symbol";
		snippet.locationFile = std::make_shared<SourceLocationFile>(FilePath(L"main.cpp"), true, true);
		snippet.locationFile->addSourceLocation(
			LOCATION_LOCAL_SYMBOL, 0, std::vector<Id>(1, id), 1, 1, 1, snippet.code.size());

		info.snippets.push_back(snippet);
	}

	info.offset = Vec2i(0, 15);

	return info;
}

Id PersistentStorage::getFileNodeId(const FilePath& filePath) const
{
	if (filePath.empty())
	{
		LOG_ERROR("No file path set");
		return 0;
	}

	std::map<FilePath, Id>::const_iterator it = m_fileNodeIds.find(filePath);

	if (it != m_fileNodeIds.end())
	{
		return it->second;
	}

	return 0;
}

std::vector<Id> PersistentStorage::getFileNodeIds(const std::vector<FilePath>& filePaths) const
{
	std::vector<Id> ids;
	for (const FilePath& path : filePaths)
	{
		ids.push_back(getFileNodeId(path));
	}
	return ids;
}

std::set<Id> PersistentStorage::getFileNodeIds(const std::set<FilePath>& filePaths) const
{
	std::set<Id> ids;
	for (const FilePath& path : filePaths)
	{
		ids.insert(getFileNodeId(path));
	}
	return ids;
}

FilePath PersistentStorage::getFileNodePath(Id fileId) const
{
	if (fileId == 0)
	{
		LOG_ERROR("No file id set");
		return FilePath();
	}

	std::map<Id, FilePath>::const_iterator it = m_fileNodePaths.find(fileId);

	if (it != m_fileNodePaths.end())
	{
		return it->second;
	}

	return FilePath();
}

bool PersistentStorage::getFilePathComplete(const FilePath& filePath) const
{
	auto it = m_fileNodeIds.find(filePath);
	if (it != m_fileNodeIds.end())
	{
		return getFileNodeComplete(it->second);
	}

	return false;
}

bool PersistentStorage::getFileNodeComplete(Id fileId) const
{
	auto it = m_fileNodeComplete.find(fileId);
	if (it != m_fileNodeComplete.end())
	{
		return it->second;
	}

	return false;
}

std::unordered_map<Id, std::set<Id>> PersistentStorage::getFileIdToIncludingFileIdMap() const
{
	std::unordered_map<Id, std::set<Id>> fileIdToIncludingFileIdMap;
	for (const StorageEdge& includeEdge : m_sqliteIndexStorage.getEdgesByType(Edge::typeToInt(Edge::EDGE_INCLUDE)))
	{
		fileIdToIncludingFileIdMap[includeEdge.targetNodeId].insert(includeEdge.sourceNodeId);
	}
	return fileIdToIncludingFileIdMap;
}

std::unordered_map<Id, std::set<Id>> PersistentStorage::getFileIdToIncludedFileIdMap() const
{
	std::unordered_map<Id, std::set<Id>> fileIdToIncludingFileIdMap;
	for (const StorageEdge& includeEdge : m_sqliteIndexStorage.getEdgesByType(Edge::typeToInt(Edge::EDGE_INCLUDE)))
	{
		fileIdToIncludingFileIdMap[includeEdge.sourceNodeId].insert(includeEdge.targetNodeId);
	}
	return fileIdToIncludingFileIdMap;
}

std::unordered_map<Id, std::set<Id>> PersistentStorage::getFileIdToImportingFileIdMap() const
{
	std::unordered_map<Id, std::set<Id>> fileIdToImportingFileIdMap;
	{
		std::vector<Id> importedElementIds;
		std::map<Id, std::set<Id>> elementIdToImportingFileIds;

		for (const StorageEdge& importEdge : m_sqliteIndexStorage.getEdgesByType(Edge::typeToInt(Edge::EDGE_IMPORT)))
		{
			importedElementIds.push_back(importEdge.targetNodeId);
			elementIdToImportingFileIds[importEdge.targetNodeId].insert(importEdge.sourceNodeId);
		}

		std::unordered_map<Id, Id> importedElementIdToFileNodeId;
		{
			std::vector<Id> importedSourceLocationIds;
			std::unordered_map<Id, Id> importedSourceLocationToElementIds;
			for (const StorageOccurrence& occurrence: m_sqliteIndexStorage.getOccurrencesForElementIds(importedElementIds))
			{
				importedSourceLocationIds.push_back(occurrence.sourceLocationId);
				importedSourceLocationToElementIds[occurrence.sourceLocationId] = occurrence.elementId;
			}

			for (const StorageSourceLocation& sourceLocation:
					m_sqliteIndexStorage.getAllByIds<StorageSourceLocation>(importedSourceLocationIds))
			{
				auto it = importedSourceLocationToElementIds.find(sourceLocation.id);
				if (it != importedSourceLocationToElementIds.end())
				{
					importedElementIdToFileNodeId[it->second] = sourceLocation.fileNodeId;
				}
			}
		}

		for (const auto& it: elementIdToImportingFileIds)
		{
			auto importedFileIt = importedElementIdToFileNodeId.find(it.first);
			if (importedFileIt != importedElementIdToFileNodeId.end())
			{
				fileIdToImportingFileIdMap[importedFileIt->second].insert(it.second.begin(), it.second.end());
			}
		}
	}
	return fileIdToImportingFileIdMap;
}

std::set<Id> PersistentStorage::getReferenced(
	const std::set<Id>& ids, std::unordered_map<Id, std::set<Id>> idToReferencingIdMap) const
{
	std::unordered_map<Id, std::set<Id>> idToReferencedIdMap;
	for (const auto& it: idToReferencingIdMap)
	{
		for (Id referencingId: it.second)
		{
			idToReferencedIdMap[referencingId].insert(it.first);
		}
	}

	return getReferencing(ids, idToReferencedIdMap);
}

std::set<Id> PersistentStorage::getReferencing(
	const std::set<Id>& ids, std::unordered_map<Id, std::set<Id>> idToReferencingIdMap) const
{
	std::set<Id> referencingIds;

	std::set<Id> processingIds = ids;
	std::set<Id> processedIds;

	while (!processingIds.empty())
	{
		std::set<Id> tempIds = processingIds;
		utility::append(processedIds, processingIds);
		processingIds.clear();

		for (Id id: tempIds)
		{
			utility::append(referencingIds, idToReferencingIdMap[id]);
			for (Id referencingId: idToReferencingIdMap[id])
			{
				if (processedIds.find(referencingId) == processedIds.end())
				{
					processingIds.insert(referencingId);
				}
			}
		}
	}

	return referencingIds;

}

std::set<FilePath> PersistentStorage::getReferencedByIncludes(const std::set<FilePath>& filePaths)
{
	const std::set<Id> ids = getReferenced(getFileNodeIds(filePaths), getFileIdToIncludingFileIdMap());

	std::set<FilePath> paths;
	for (Id id: ids)
	{ // TODO: performance optimize: use just one request for all ids!
		paths.insert(getFileNodePath(id));
	}

	return paths;
}

std::set<FilePath> PersistentStorage::getReferencedByImports(const std::set<FilePath>& filePaths)
{
	const std::set<Id> ids = getReferenced(getFileNodeIds(filePaths), getFileIdToImportingFileIdMap());

	std::set<FilePath> paths;
	for (Id id: ids)
	{
		paths.insert(getFileNodePath(id));
	}

	return paths;
}

std::set<FilePath> PersistentStorage::getReferencingByIncludes(const std::set<FilePath>& filePaths)
{
	const std::set<Id> ids = getReferencing(getFileNodeIds(filePaths), getFileIdToIncludingFileIdMap());

	std::set<FilePath> paths;
	for (Id id: ids)
	{
		paths.insert(getFileNodePath(id));
	}

	return paths;
}

std::set<FilePath> PersistentStorage::getReferencingByImports(const std::set<FilePath>& filePaths)
{
	const std::set<Id> ids = getReferencing(getFileNodeIds(filePaths), getFileIdToImportingFileIdMap());

	std::set<FilePath> paths;
	for (Id id: ids)
	{
		paths.insert(getFileNodePath(id));
	}

	return paths;
}

void PersistentStorage::addNodesToGraph(const std::vector<Id>& newNodeIds, Graph* graph, bool addChildCount) const
{
	TRACE();

	std::vector<Id> nodeIds;
	if (graph->getNodeCount())
	{
		for (Id id : newNodeIds)
		{
			if (!graph->getNodeById(id))
			{
				nodeIds.push_back(id);
			}
		}
	}
	else
	{
		nodeIds = newNodeIds;
	}

	if (nodeIds.size() == 0)
	{
		return;
	}

	for (const StorageNode& storageNode : m_sqliteIndexStorage.getAllByIds<StorageNode>(nodeIds))
	{
		const NodeType type(utility::intToType(storageNode.type));
		if (type.isFile())
		{
			const FilePath filePath(NameHierarchy::deserialize(storageNode.serializedName).getRawName());

			bool defined = false;
			auto it = m_fileNodeComplete.find(storageNode.id);
			if (it != m_fileNodeComplete.end())
			{
				defined = it->second;
			}

			Node* node = graph->createNode(
				storageNode.id,
				type,
				NameHierarchy(filePath.fileName(), NAME_DELIMITER_FILE),
				defined
			);
			node->addComponent(std::make_shared<TokenComponentFilePath>(filePath));
			node->setExplicit(defined);
		}
		else
		{
			const NameHierarchy nameHierarchy = NameHierarchy::deserialize(storageNode.serializedName);

			DefinitionKind defKind = DEFINITION_NONE;
			auto it = m_symbolDefinitionKinds.find(storageNode.id);
			if (it != m_symbolDefinitionKinds.end())
			{
				defKind = it->second;
			}

			Node* node = graph->createNode(
				storageNode.id,
				type,
				nameHierarchy,
				defKind != DEFINITION_NONE
			);

			if (defKind == DEFINITION_IMPLICIT)
			{
				node->setImplicit(true);
			}
			else if (defKind == DEFINITION_EXPLICIT)
			{
				node->setExplicit(true);
			}

			if (addChildCount)
			{
				node->setChildCount(m_hierarchyCache.getFirstChildIdsCountForNodeId(storageNode.id));
			}
		}
	}
}

void PersistentStorage::addEdgesToGraph(const std::vector<Id>& newEdgeIds, Graph* graph) const
{
	TRACE();

	std::vector<Id> edgeIds;
	for (Id id : newEdgeIds)
	{
		if (!graph->getEdgeById(id))
		{
			edgeIds.push_back(id);
		}
	}

	if (edgeIds.size() == 0)
	{
		return;
	}

	for (const StorageEdge& storageEdge : m_sqliteIndexStorage.getAllByIds<StorageEdge>(edgeIds))
	{
		Node* sourceNode = graph->getNodeById(storageEdge.sourceNodeId);
		Node* targetNode = graph->getNodeById(storageEdge.targetNodeId);

		if (sourceNode && targetNode)
		{
			Edge::EdgeType type = Edge::intToType(storageEdge.type);
			Id edgeId = storageEdge.id;
			if (type & Edge::EDGE_MEMBER && m_memberEdgeIdOrderMap.size())
			{
				auto it = m_memberEdgeIdOrderMap.find(edgeId);
				if (it != m_memberEdgeIdOrderMap.end())
				{
					edgeId = it->second;
				}
			}

			graph->createEdge(edgeId, type, sourceNode, targetNode);
		}
		else
		{
			LOG_ERROR("Can't add edge because nodes are not present");
		}
	}
}

void PersistentStorage::addNodesWithParentsAndEdgesToGraph(
	const std::vector<Id>& nodeIds, const std::vector<Id>& edgeIds, Graph* graph, bool addChildCount
) const
{
	TRACE();

	std::set<Id> allNodeIds(nodeIds.begin(), nodeIds.end());
	std::set<Id> allEdgeIds(edgeIds.begin(), edgeIds.end());

	if (edgeIds.size() > 0)
	{
		for (const StorageEdge& storageEdge : m_sqliteIndexStorage.getAllByIds<StorageEdge>(edgeIds))
		{
			allNodeIds.insert(storageEdge.sourceNodeId);
			allNodeIds.insert(storageEdge.targetNodeId);
		}
	}

	std::set<Id> parentNodeIds;
	for (Id nodeId : allNodeIds)
	{
		m_hierarchyCache.addAllVisibleParentIdsForNodeId(nodeId, &parentNodeIds, &allEdgeIds);
	}

	allNodeIds.insert(parentNodeIds.begin(), parentNodeIds.end());

	addNodesToGraph(utility::toVector(allNodeIds), graph, addChildCount);
	addEdgesToGraph(utility::toVector(allEdgeIds), graph);
}

void PersistentStorage::addAggregationEdgesToGraph(
	Id nodeId, const std::vector<StorageEdge>& edgesToAggregate, Graph* graph) const
{
	TRACE();

	struct EdgeInfo
	{
		Id edgeId;
		bool forward;
	};

	// build aggregation edges:
	// get all children of the active node
	std::set<Id> childNodeIdsSet, edgeIdsSet;
	m_hierarchyCache.addAllChildIdsForNodeId(nodeId, &childNodeIdsSet, &edgeIdsSet);
	const std::vector<Id> childNodeIds = utility::toVector(childNodeIdsSet);
	if (childNodeIds.size() == 0 && edgesToAggregate.size() == 0)
	{
		return;
	}

	// get all edges of the children
	std::map<Id, std::vector<EdgeInfo>> connectedNodeIds;
	for (const StorageEdge& edge : edgesToAggregate)
	{
		bool isSource = nodeId == edge.sourceNodeId;
		EdgeInfo edgeInfo;
		edgeInfo.edgeId = edge.id;
		edgeInfo.forward = isSource;
		connectedNodeIds[isSource ? edge.targetNodeId : edge.sourceNodeId].push_back(edgeInfo);
	}

	const std::vector<StorageEdge> outgoingEdges = m_sqliteIndexStorage.getEdgesBySourceIds(childNodeIds);
	for (const StorageEdge& outEdge : outgoingEdges)
	{
		EdgeInfo edgeInfo;
		edgeInfo.edgeId = outEdge.id;
		edgeInfo.forward = true;
		connectedNodeIds[outEdge.targetNodeId].push_back(edgeInfo);
	}

	const std::vector<StorageEdge> incomingEdges = m_sqliteIndexStorage.getEdgesByTargetIds(childNodeIds);
	for (const StorageEdge& inEdge : incomingEdges)
	{
		EdgeInfo edgeInfo;
		edgeInfo.edgeId = inEdge.id;
		edgeInfo.forward = false;
		connectedNodeIds[inEdge.sourceNodeId].push_back(edgeInfo);
	}

	// get all parent nodes of all connected nodes (up to last level except namespace/undefined)
	const Id nodeParentNodeId = m_hierarchyCache.getLastVisibleParentNodeId(nodeId);

	std::map<Id, std::vector<EdgeInfo>> connectedParentNodeIds;
	for (const std::pair<Id, std::vector<EdgeInfo>>& p : connectedNodeIds)
	{
		const Id parentNodeId = m_hierarchyCache.getLastVisibleParentNodeId(p.first);

		if (parentNodeId != nodeParentNodeId)
		{
			utility::append(connectedParentNodeIds[parentNodeId], p.second);
		}
	}

	// add hierarchies of these parents
	std::vector<Id> nodeIdsToAdd;
	for (const std::pair<Id, std::vector<EdgeInfo>>& p : connectedParentNodeIds)
	{
		const Id aggregationTargetNodeId = p.first;
		if (!graph->getNodeById(aggregationTargetNodeId))
		{
			nodeIdsToAdd.push_back(aggregationTargetNodeId);
		}
	}
	addNodesWithParentsAndEdgesToGraph(nodeIdsToAdd, std::vector<Id>(), graph, true);

	// create aggregation edges between parents and active node
	Node* sourceNode = graph->getNodeById(nodeId);
	for (const std::pair<Id, std::vector<EdgeInfo>>& p : connectedParentNodeIds)
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

		// Set first bit to 1 to avoid collisions
		const Id aggregationId = ~(~Id(0) >> 1) + *componentAggregation->getAggregationIds().begin();

		Edge* edge = graph->createEdge(aggregationId, Edge::EDGE_AGGREGATION, sourceNode, targetNode);
		edge->addComponent(componentAggregation);
	}
}

void PersistentStorage::addFileContentsToGraph(Id fileId, Graph* graph) const
{
	FilePath path = getFileNodePath(fileId);
	if (path.empty())
	{
		return;
	}

	std::vector<Id> tokenIds;
	std::set<Id> tokenIdsSet;

	std::shared_ptr<SourceLocationFile> locationFile = m_sqliteIndexStorage.getSourceLocationsForFile(path);
	locationFile->forEachStartSourceLocation(
		[this, &tokenIds, &tokenIdsSet](SourceLocation* location)
		{
			if (location->getType() != LOCATION_TOKEN)
			{
				return;
			}

			for (Id tokenId : location->getTokenIds())
			{
				if (tokenIdsSet.insert(tokenId).second)
				{
					auto it = m_symbolDefinitionKinds.find(tokenId);
					if (it == m_symbolDefinitionKinds.end() || it->second != DEFINITION_IMPLICIT)
					{
						tokenIds.push_back(tokenId);
					}
				}
			}
		}
	);

	addNodesWithParentsAndEdgesToGraph(tokenIds, { }, graph, true);

	Node* fileNode = graph->getNodeById(fileId);
	Id memberEdgeId = 0;
	for (Id tokenId : tokenIds)
	{
		Id nodeId = m_hierarchyCache.getLastVisibleParentNodeId(tokenId);
		Node* node = graph->getNodeById(nodeId);
		if (node && !node->getMemberEdge())
		{
			// Set first bit to 1 to avoid collisions
			graph->createEdge(~(~Id(0) >> 1) + memberEdgeId++, Edge::EDGE_MEMBER, fileNode, node);
		}
	}

	fileNode->setChildCount(memberEdgeId);
}

void PersistentStorage::addComponentAccessToGraph(Graph* graph) const
{
	TRACE();

	std::vector<Id> nodeIds;
	graph->forEachNode(
		[&nodeIds](Node* node)
		{
			nodeIds.push_back(node->getId());
		}
	);

	std::vector<StorageComponentAccess> accesses = m_sqliteIndexStorage.getComponentAccessesByNodeIds(nodeIds);
	for (const StorageComponentAccess& access : accesses)
	{
		if (access.nodeId != 0)
		{
			graph->getNodeById(access.nodeId)->addComponent(
				std::make_shared<TokenComponentAccess>(intToAccessKind(access.type)));
		}
	}
}

void PersistentStorage::addCompleteFlagsToSourceLocationCollection(SourceLocationCollection* collection) const
{
	TRACE();

	collection->forEachSourceLocationFile(
		[this](std::shared_ptr<SourceLocationFile> file)
		{
			file->setIsComplete(getFilePathComplete(file->getFilePath()));
		}
	);
}

void PersistentStorage::addInheritanceChainsToGraph(const std::vector<Id>& activeNodeIds, Graph* graph) const
{
	TRACE();

	std::set<Id> activeNodeIdsSet;
	for (Id activeNodeId : activeNodeIds)
	{
		std::set<Id> visibleParentIds, edgeIds;
		visibleParentIds.insert(activeNodeId);
		m_hierarchyCache.addAllVisibleParentIdsForNodeId(activeNodeId, &visibleParentIds, &edgeIds);

		for (Id nodeId : visibleParentIds)
		{
			Node* node = graph->getNodeById(nodeId);
			if (node && node->getType().isInheritable())
			{
				activeNodeIdsSet.insert(node->getId());
			}
		}
	}

	std::set<Id> nodeIdsSet;
	graph->forEachNode(
		[&nodeIdsSet, &activeNodeIdsSet](Node* node)
		{
			if (node->getType().isInheritable() && activeNodeIdsSet.find(node->getId()) == activeNodeIdsSet.end())
			{
				nodeIdsSet.insert(node->getId());
			}
		}
	);

	std::vector<std::set<Id>*> nodeIdSets;
	nodeIdSets.push_back(&activeNodeIdsSet);
	nodeIdSets.push_back(&nodeIdsSet);

	size_t inheritanceEdgeCount = 1;

	for (size_t i = 0; i < nodeIdSets.size(); i++)
	{
		for (const Id nodeId : *nodeIdSets[i])
		{
			for (const std::tuple<Id, Id, std::vector<Id>>& edge :
				m_hierarchyCache.getInheritanceEdgesForNodeId(nodeId, *nodeIdSets[(i + 1) % 2]))
			{
				const Id sourceId = std::get<0>(edge);
				const Id targetId = std::get<1>(edge);
				const std::vector<Id> edgeIds = std::get<2>(edge);

				if (!edgeIds.size() || (edgeIds.size() == 1 && graph->getEdgeById(edgeIds[0])))
				{
					continue;
				}

				// Set first 2 bits to 1 to avoid collisions
				const Id inheritanceEdgeId = ~(~Id(0) >> 2) + inheritanceEdgeCount++;

				Edge* inheritanceEdge = graph->createEdge(
					inheritanceEdgeId, Edge::EDGE_INHERITANCE, graph->getNodeById(sourceId), graph->getNodeById(targetId));

				inheritanceEdge->addComponent(std::make_shared<TokenComponentInheritanceChain>(edgeIds));
			}
		}
	}
}

void PersistentStorage::buildFilePathMaps()
{
	TRACE();

	for (StorageFile& file: m_sqliteIndexStorage.getAll<StorageFile>())
	{
		const FilePath path(file.filePath);

		m_fileNodeIds.emplace(path, file.id);
		m_fileNodePaths.emplace(file.id, path);
		m_fileNodeComplete.emplace(file.id, file.complete);

		if (!m_hasJavaFiles && path.extension() == L".java")
		{
			m_hasJavaFiles = true;
		}
	}

	for (StorageSymbol& symbol : m_sqliteIndexStorage.getAll<StorageSymbol>())
	{
		m_symbolDefinitionKinds.emplace(symbol.id, intToDefinitionKind(symbol.definitionKind));
	}
}

void PersistentStorage::buildSearchIndex()
{
	TRACE();

	const FilePath dbPath = getDbFilePath();

	for (StorageNode& node : m_sqliteIndexStorage.getAll<StorageNode>())
	{
		NodeType type = utility::intToType(node.type);
		if (type.isFile())
		{
			auto it = m_fileNodePaths.find(node.id);
			if (it != m_fileNodePaths.end())
			{
				FilePath filePath(it->second);

				if (filePath.exists())
				{
					filePath.makeRelativeTo(dbPath);
				}

				m_fileIndex.addNode(node.id, filePath.wstr(), type);
			}
		}
		else
		{
			auto it = m_symbolDefinitionKinds.find(node.id);
			const DefinitionKind defKind = (it != m_symbolDefinitionKinds.end() ? it->second : DEFINITION_NONE);
			if (defKind != DEFINITION_IMPLICIT)
			{
				const NameHierarchy nameHierarchy = NameHierarchy::deserialize(node.serializedName);

				// we don't use the signature here, so elements with the same signature share the same node.
				std::wstring name = nameHierarchy.getQualifiedName();

				// replace template arguments with .. to avoid clutter in search results and have different
				// template specializations share the same node.
				if (defKind == DEFINITION_NONE && nameHierarchy.getDelimiter() == NAME_DELIMITER_CXX)
				{
					name = utility::replaceBetween(name, L'<', L'>', L"..");
				}

				m_symbolIndex.addNode(node.id, name, type);
			}
		}
	}

	m_symbolIndex.finishSetup();
	m_fileIndex.finishSetup();
}

void PersistentStorage::buildFullTextSearchIndex() const
{
	TRACE();

	TextCodec codec(ApplicationSettings::getInstance()->getTextEncoding());

	m_fullTextSearchCodec = codec.getName();

	m_fullTextSearchIndex.clear();
	for (StorageFile& file : m_sqliteIndexStorage.getAll<StorageFile>())
	{

		m_fullTextSearchIndex.addFile(
			file.id,
			codec.decode(m_sqliteIndexStorage.getFileContentById(file.id)->getText())
		);
	}
}

void PersistentStorage::buildMemberEdgeIdOrderMap()
{
	TRACE();

	if (!m_hasJavaFiles)
	{
		return;
	}

	std::vector<Id> childNodeIds;
	std::unordered_map<Id, Id> childIdToMemberEdgeIdMap;

	for (const StorageEdge& edge : m_sqliteIndexStorage.getEdgesByType(Edge::typeToInt(Edge::EDGE_MEMBER)))
	{
		childNodeIds.push_back(edge.targetNodeId);
		childIdToMemberEdgeIdMap.emplace(edge.targetNodeId, edge.id);
	}

	std::vector<Id> locationIds;
	std::unordered_map<Id, Id> locationIdToElementIdMap;
	for (const StorageOccurrence& occurrence: m_sqliteIndexStorage.getOccurrencesForElementIds(childNodeIds))
	{
		locationIds.push_back(occurrence.sourceLocationId);
		locationIdToElementIdMap.emplace(occurrence.sourceLocationId, occurrence.elementId);
	}

	SourceLocationCollection collection;
	for (const StorageSourceLocation& location: m_sqliteIndexStorage.getAllByIds<StorageSourceLocation>(locationIds))
	{
		const LocationType locType = intToLocationType(location.type);
		if (locType != LOCATION_TOKEN)
		{
			continue;
		}

		const FilePath path(m_fileNodePaths[location.fileNodeId]);
		if (path.extension() == L".java")
		{
			collection.addSourceLocation(
				intToLocationType(location.type),
				location.id,
				std::vector<Id>(),
				FilePath(std::to_wstring(location.fileNodeId)),
				location.startLine,
				location.startCol,
				location.endLine,
				location.endCol
			);
		}
	}

	// Set first 3 bits to 1 to avoid collisions
	Id baseId = ~(~Id(0) >> 3) + 1;

	collection.forEachSourceLocation(
		[&](SourceLocation* location)
		{
			auto it = locationIdToElementIdMap.find(location->getLocationId());
			if (it != locationIdToElementIdMap.end())
			{
				auto it2 = childIdToMemberEdgeIdMap.find(it->second);
				if (it2 != childIdToMemberEdgeIdMap.end())
				{
					if (m_memberEdgeIdOrderMap.emplace(it2->second, baseId).second)
					{
						baseId++;
					}
				}
			}
		}
	);
}

void PersistentStorage::buildHierarchyCache()
{
	TRACE();

	const std::vector<StorageEdge> memberEdges = m_sqliteIndexStorage.getEdgesByType(Edge::typeToInt(Edge::EDGE_MEMBER));

	std::vector<Id> sourceNodeIds;
	for (const StorageEdge& edge : memberEdges)
	{
		sourceNodeIds.push_back(edge.sourceNodeId);
	}

	std::map<Id, NodeType> sourceNodeTypeMap;
	for (const StorageNode& node : m_sqliteIndexStorage.getAllByIds<StorageNode>(sourceNodeIds))
	{
		sourceNodeTypeMap.emplace(node.id, utility::intToType(node.type));
	}

	for (const StorageEdge& edge : memberEdges)
	{
		bool sourceIsVisible = true;
		{
			std::map<Id, NodeType>::const_iterator it = sourceNodeTypeMap.find(edge.sourceNodeId);
			if (it != sourceNodeTypeMap.end())
			{
				sourceIsVisible = it->second.isVisibleAsParentInGraph();
			}
		}

		bool sourceIsImplicit = false;
		auto it = m_symbolDefinitionKinds.find(edge.sourceNodeId);
		if (it != m_symbolDefinitionKinds.end())
		{
			sourceIsImplicit = (it->second == DEFINITION_IMPLICIT);
		}

		bool targetIsImplicit = false;
		it = m_symbolDefinitionKinds.find(edge.targetNodeId);
		if (it != m_symbolDefinitionKinds.end())
		{
			targetIsImplicit = (it->second == DEFINITION_IMPLICIT);
		}

		m_hierarchyCache.createConnection(
			edge.id, edge.sourceNodeId, edge.targetNodeId, sourceIsVisible, sourceIsImplicit, targetIsImplicit);
	}

	std::vector<StorageEdge> inheritanceEdges = m_sqliteIndexStorage.getEdgesByType(Edge::typeToInt(Edge::EDGE_INHERITANCE));
	for (const StorageEdge& edge : inheritanceEdges)
	{
		m_hierarchyCache.createInheritance(edge.id, edge.sourceNodeId, edge.targetNodeId);
	}
}
