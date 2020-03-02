#include "PersistentStorage.h"

#include <queue>
#include <sstream>

#include "AccessKind.h"
#include "ApplicationSettings.h"
#include "ElementComponentKind.h"
#include "FileInfo.h"
#include "FilePath.h"
#include "Graph.h"
#include "MessageErrorCountUpdate.h"
#include "MessageStatus.h"
#include "NodeTypeSet.h"
#include "ParseLocation.h"
#include "SourceLocationCollection.h"
#include "SourceLocationFile.h"
#include "TextAccess.h"
#include "TextCodec.h"
#include "TimeStamp.h"
#include "TokenComponentAccess.h"
#include "TokenComponentAggregation.h"
#include "TokenComponentFilePath.h"
#include "TokenComponentInheritanceChain.h"
#include "TokenComponentIsAmbiguous.h"
#include "UnorderedCache.h"
#include "logging.h"
#include "tracing.h"
#include "utility.h"
#include "utilityApp.h"

PersistentStorage::PersistentStorage(const FilePath& dbPath, const FilePath& bookmarkPath)
	: m_sqliteIndexStorage(dbPath), m_sqliteBookmarkStorage(bookmarkPath)
{
	m_commandIndex.addNode(0, SearchMatch::getCommandName(SearchMatch::COMMAND_ALL));
	m_commandIndex.addNode(0, SearchMatch::getCommandName(SearchMatch::COMMAND_ERROR));
	m_commandIndex.addNode(0, SearchMatch::getCommandName(SearchMatch::COMMAND_LEGEND));

	for (const NodeType& nodeType: NodeTypeSet::all().getNodeTypes())
	{
		if (nodeType.hasSearchFilter())
		{
			m_commandIndex.addNode(0, nodeType.getReadableTypeWString());
		}
	}

	m_commandIndex.finishSetup();
}

std::pair<Id, bool> PersistentStorage::addNode(const StorageNodeData& data)
{
	return std::make_pair(m_sqliteIndexStorage.addNode(data), true);
}

std::vector<Id> PersistentStorage::addNodes(const std::vector<StorageNode>& nodes)
{
	return m_sqliteIndexStorage.addNodes(nodes);
}

void PersistentStorage::addSymbol(const StorageSymbol& data)
{
	m_sqliteIndexStorage.addSymbol(data);
}

void PersistentStorage::addSymbols(const std::vector<StorageSymbol>& symbols)
{
	m_sqliteIndexStorage.addSymbols(symbols);
}

void PersistentStorage::addFile(const StorageFile& data)
{
	const StorageFile storedFile = m_sqliteIndexStorage.getFirstById<StorageFile>(data.id);

	if (storedFile.id == 0)
	{
		m_sqliteIndexStorage.addFile(data);
	}
	else
	{
		if (!storedFile.indexed && data.indexed)
		{
			m_sqliteIndexStorage.setFileIndexed(storedFile.id, data.indexed);
		}

		if (storedFile.complete != data.complete)
		{
			m_sqliteIndexStorage.setFileCompleteIfNoError(
				storedFile.id, storedFile.filePath, data.complete);
		}
	}
}

Id PersistentStorage::addEdge(const StorageEdgeData& data)
{
	return m_sqliteIndexStorage.addEdge(data);
}

std::vector<Id> PersistentStorage::addEdges(const std::vector<StorageEdge>& edges)
{
	return m_sqliteIndexStorage.addEdges(edges);
}

Id PersistentStorage::addLocalSymbol(const StorageLocalSymbolData& data)
{
	return m_sqliteIndexStorage.addLocalSymbol(data);
}

std::vector<Id> PersistentStorage::addLocalSymbols(const std::set<StorageLocalSymbol>& symbols)
{
	return m_sqliteIndexStorage.addLocalSymbols(symbols);
}

Id PersistentStorage::addSourceLocation(const StorageSourceLocationData& data)
{
	return m_sqliteIndexStorage.addSourceLocation(data);
}

std::vector<Id> PersistentStorage::addSourceLocations(const std::vector<StorageSourceLocation>& locations)
{
	return m_sqliteIndexStorage.addSourceLocations(locations);
}

void PersistentStorage::addOccurrence(const StorageOccurrence& data)
{
	m_sqliteIndexStorage.addOccurrence(data);
}

void PersistentStorage::addOccurrences(const std::vector<StorageOccurrence>& occurrences)
{
	m_sqliteIndexStorage.addOccurrences(occurrences);
}

void PersistentStorage::addComponentAccess(const StorageComponentAccess& componentAccess)
{
	m_sqliteIndexStorage.addComponentAccess(componentAccess);
}

void PersistentStorage::addComponentAccesses(const std::vector<StorageComponentAccess>& componentAccesses)
{
	m_sqliteIndexStorage.addComponentAccesses(componentAccesses);
}

void PersistentStorage::addElementComponent(const StorageElementComponent& component)
{
	m_sqliteIndexStorage.addElementComponent(component);
}

void PersistentStorage::addElementComponents(const std::vector<StorageElementComponent>& components)
{
	m_sqliteIndexStorage.addElementComponents(components);
}

Id PersistentStorage::addError(const StorageErrorData& data)
{
	return m_sqliteIndexStorage.addError(data).id;
}

void PersistentStorage::removeElement(const Id id)
{
	m_sqliteIndexStorage.removeElement(id);
}

void PersistentStorage::removeElements(const std::vector<Id>& ids)
{
	m_sqliteIndexStorage.removeElements(ids);
}

void PersistentStorage::removeOccurrence(const StorageOccurrence& occurrence)
{
	m_sqliteIndexStorage.removeOccurrence(occurrence);
}

void PersistentStorage::removeOccurrences(const std::vector<StorageOccurrence>& occurrences)
{
	m_sqliteIndexStorage.removeOccurrences(occurrences);
}

void PersistentStorage::removeElementsWithoutOccurrences(const std::vector<Id>& elementIds)
{
	m_sqliteIndexStorage.removeElementsWithoutOccurrences(elementIds);
}

const std::vector<StorageNode>& PersistentStorage::getStorageNodes() const
{
	return m_storageData.nodes = m_sqliteIndexStorage.getAll<StorageNode>();
}

const std::vector<StorageFile>& PersistentStorage::getStorageFiles() const
{
	return m_storageData.files = m_sqliteIndexStorage.getAll<StorageFile>();
}

const std::vector<StorageSymbol>& PersistentStorage::getStorageSymbols() const
{
	return m_storageData.symbols = m_sqliteIndexStorage.getAll<StorageSymbol>();
}

const std::vector<StorageEdge>& PersistentStorage::getStorageEdges() const
{
	return m_storageData.edges = m_sqliteIndexStorage.getAll<StorageEdge>();
}

const std::set<StorageLocalSymbol>& PersistentStorage::getStorageLocalSymbols() const
{
	return m_storageData.locals = utility::toSet(m_sqliteIndexStorage.getAll<StorageLocalSymbol>());
}

const std::set<StorageSourceLocation>& PersistentStorage::getStorageSourceLocations() const
{
	return m_storageData.locations = utility::toSet(
			   m_sqliteIndexStorage.getAll<StorageSourceLocation>());
}

const std::set<StorageOccurrence>& PersistentStorage::getStorageOccurrences() const
{
	return m_storageData.occurrences = utility::toSet(
			   m_sqliteIndexStorage.getAll<StorageOccurrence>());
}

const std::set<StorageComponentAccess>& PersistentStorage::getComponentAccesses() const
{
	return m_storageData.accesses = utility::toSet(
			   m_sqliteIndexStorage.getAll<StorageComponentAccess>());
}

const std::set<StorageElementComponent>& PersistentStorage::getElementComponents() const
{
	return m_storageData.components = utility::toSet(
			   m_sqliteIndexStorage.getAll<StorageElementComponent>());
}

const std::vector<StorageError>& PersistentStorage::getErrors() const
{
	std::vector<StorageError> errors;
	for (const StorageError& error: m_sqliteIndexStorage.getAll<StorageError>())
	{
		errors.emplace_back(error);
	}
	return m_storageData.errors = errors;
}

void PersistentStorage::startInjection()
{
	beforeErrorRecording();

	m_sqliteIndexStorage.beginTransaction();
}

void PersistentStorage::finishInjection()
{
	m_sqliteIndexStorage.commitTransaction();

	afterErrorRecording();
}

void PersistentStorage::rollbackInjection()
{
	m_sqliteIndexStorage.rollbackTransaction();

	afterErrorRecording();
}

void PersistentStorage::beforeErrorRecording()
{
	m_preInjectionErrorCount = m_sqliteIndexStorage.getErrorCount();

	if (!m_preIndexingErrorCountSet)
	{
		m_preIndexingErrorCount = m_preInjectionErrorCount;
		m_preIndexingErrorCountSet = true;
	}
}

void PersistentStorage::afterErrorRecording()
{
	std::vector<ErrorInfo> errors = m_sqliteIndexStorage.getAllErrorInfos();
	if (m_preInjectionErrorCount < errors.size())
	{
		ErrorCountInfo errorCount(errors);
		errors.erase(
			errors.begin(), errors.begin() + m_preInjectionErrorCount - m_preIndexingErrorCount);
		MessageErrorCountUpdate(errorCount, errors).dispatch();
		m_preIndexingErrorCount = 0;
	}
}

void PersistentStorage::setMode(const SqliteIndexStorage::StorageModeType mode)
{
	m_sqliteIndexStorage.setMode(mode);
}

FilePath PersistentStorage::getIndexDbFilePath() const
{
	return m_sqliteIndexStorage.getDbFilePath();
}

FilePath PersistentStorage::getBookmarkDbFilePath() const
{
	return m_sqliteBookmarkStorage.getDbFilePath();
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

void PersistentStorage::updateVersion()
{
	m_sqliteIndexStorage.setVersion(m_sqliteIndexStorage.getStaticVersion());
	if (m_sqliteBookmarkStorage.isEmpty())
	{
		m_sqliteBookmarkStorage.setVersion(m_sqliteBookmarkStorage.getStaticVersion());
	}
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
	m_lowerCasefileNodeIds.clear();
	m_fileNodePaths.clear();
	m_fileNodeComplete.clear();
	m_fileNodeIndexed.clear();
	m_fileNodeLanguage.clear();
	m_symbolDefinitionKinds.clear();

	m_hierarchyCache.clear();
	m_fullTextSearchIndex.clear();
	m_fullTextSearchCodec = "";
}

std::set<FilePath> PersistentStorage::getReferenced(const std::set<FilePath>& filePaths) const
{
	TRACE();
	std::set<FilePath> referenced;

	utility::append(referenced, getReferencedByIncludes(filePaths));
	utility::append(referenced, getReferencedByImports(filePaths));

	return referenced;
}

std::set<FilePath> PersistentStorage::getReferencing(const std::set<FilePath>& filePaths) const
{
	TRACE();
	std::set<FilePath> referencing;

	utility::append(referencing, getReferencingByIncludes(filePaths));
	utility::append(referencing, getReferencingByImports(filePaths));

	return referencing;
}

void PersistentStorage::clearAllErrors()
{
	TRACE();

	m_sqliteIndexStorage.removeAllErrors();
}

void PersistentStorage::clearFileElements(
	const std::vector<FilePath>& filePaths, std::function<void(int)> updateStatusCallback)
{
	TRACE();

	std::vector<Id> fileNodeIds;
	for (const StorageFile& file: m_sqliteIndexStorage.getFilesByPaths(filePaths))
	{
		fileNodeIds.push_back(file.id);
	}

	if (!fileNodeIds.empty())
	{
		m_sqliteIndexStorage.beginTransaction();
		m_sqliteIndexStorage.removeElementsWithLocationInFiles(fileNodeIds, updateStatusCallback);
		m_sqliteIndexStorage.removeElements(fileNodeIds);
		m_sqliteIndexStorage.commitTransaction();
		updateStatusCallback(100);
	}
}

std::vector<FileInfo> PersistentStorage::getFileInfoForAllFiles() const
{
	TRACE();

	std::vector<FileInfo> fileInfos;

	m_sqliteIndexStorage.forEach<StorageFile>([&](StorageFile&& file) {
		boost::posix_time::ptime modificationTime = boost::posix_time::not_a_date_time;
		if (file.modificationTime != "not-a-date-time")
		{
			modificationTime = boost::posix_time::time_from_string(file.modificationTime);
		}

		fileInfos.emplace_back(FilePath(file.filePath), modificationTime);
	});

	return fileInfos;
}

std::set<FilePath> PersistentStorage::getIncompleteFiles() const
{
	TRACE();

	std::set<FilePath> incompleteFiles;
	for (auto p: m_fileNodeComplete)
	{
		if (p.second == false)
		{
			incompleteFiles.insert(getFileNodePath(p.first));
		}
	}

	return incompleteFiles;
}

bool PersistentStorage::getFilePathIndexed(const FilePath& path) const
{
	Id fileId = getFileNodeId(path);
	if (fileId)
	{
		return getFileNodeIndexed(fileId);
	}

	return false;
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

	m_sqliteIndexStorage.setTime();
	m_sqliteIndexStorage.optimizeMemory();

	m_sqliteBookmarkStorage.optimizeMemory();
}

Id PersistentStorage::getNodeIdForFileNode(const FilePath& filePath) const
{
	return getFileNodeId(filePath);
}

Id PersistentStorage::getNodeIdForNameHierarchy(const NameHierarchy& nameHierarchy) const
{
	return m_sqliteIndexStorage.getNodeBySerializedName(NameHierarchy::serialize(nameHierarchy)).id;
}

std::vector<Id> PersistentStorage::getNodeIdsForNameHierarchies(
	const std::vector<NameHierarchy> nameHierarchies) const
{
	std::vector<Id> nodeIds;
	for (const NameHierarchy& name: nameHierarchies)
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

	return NameHierarchy::deserialize(
		m_sqliteIndexStorage.getFirstById<StorageNode>(nodeId).serializedName);
}

std::vector<NameHierarchy> PersistentStorage::getNameHierarchiesForNodeIds(
	const std::vector<Id>& nodeIds) const
{
	TRACE();

	std::vector<NameHierarchy> nameHierarchies;
	for (const StorageNode& storageNode: m_sqliteIndexStorage.getAllByIds<StorageNode>(nodeIds))
	{
		nameHierarchies.push_back(NameHierarchy::deserialize(storageNode.serializedName));
	}
	return nameHierarchies;
}

std::map<Id, std::pair<Id, NameHierarchy>> PersistentStorage::getNodeIdToParentFileMap(
	const std::vector<Id>& nodeIds) const
{
	std::map<Id, std::pair<Id, NameHierarchy>> nodeIdToParentFileMap;

	std::shared_ptr<SourceLocationCollection> locations =
		m_sqliteIndexStorage.getSourceLocationsForElementIds(nodeIds);

	// prefer scope locations if available
	locations->forEachSourceLocation([this, &nodeIdToParentFileMap](SourceLocation* location) {
		if (location->isStartLocation() && location->isScopeLocation())
		{
			for (Id tokenId: location->getTokenIds())
			{
				nodeIdToParentFileMap.emplace(
					tokenId,
					std::make_pair(
						getFileNodeId(location->getFilePath()),
						NameHierarchy(location->getFilePath().wstr(), NAME_DELIMITER_FILE)));
			}
		}
	});

	// fill in missing ones
	locations->forEachSourceLocation([this, &nodeIdToParentFileMap](SourceLocation* location) {
		if (!location->isStartLocation())
		{
			return;
		}

		for (Id tokenId: location->getTokenIds())
		{
			if (nodeIdToParentFileMap.find(tokenId) == nodeIdToParentFileMap.end())
			{
				nodeIdToParentFileMap.emplace(
					tokenId,
					std::make_pair(
						getFileNodeId(location->getFilePath()),
						NameHierarchy(location->getFilePath().wstr(), NAME_DELIMITER_FILE)));
			}
		}
	});

	return nodeIdToParentFileMap;
}

NodeType PersistentStorage::getNodeTypeForNodeWithId(Id nodeId) const
{
	return NodeType(intToNodeKind(m_sqliteIndexStorage.getFirstById<StorageNode>(nodeId).type));
}

StorageEdge PersistentStorage::getEdgeById(Id edgeId) const
{
	return m_sqliteIndexStorage.getEdgeById(edgeId);
}

std::shared_ptr<SourceLocationCollection> PersistentStorage::getFullTextSearchLocations(
	const std::wstring& searchTerm, bool caseSensitive) const
{
	TRACE();

	std::shared_ptr<SourceLocationCollection> collection =
		std::make_shared<SourceLocationCollection>();
	if (searchTerm.empty())
	{
		return collection;
	}

	const TextCodec codec(ApplicationSettings::getInstance()->getTextEncoding());
	{
		std::lock_guard<std::mutex> lock(m_fullTextSearchMutex);

		if (m_fullTextSearchCodec != codec.getName())
		{
			MessageStatus(L"Building fulltext search index", false, true).dispatch();
			buildFullTextSearchIndex();
		}
	}

	MessageStatus(
		std::wstring(L"Searching fulltext (case-") +
			(caseSensitive ? L"sensitive" : L"insensitive") + L"): " + searchTerm,
		false,
		true)
		.dispatch();

	{
		std::vector<std::shared_ptr<std::thread>> threads;
		std::mutex collectionMutex;
		for (std::vector<FullTextSearchResult> fileResults: utility::splitToEqualySizedParts(
				 m_fullTextSearchIndex.searchForTerm(searchTerm), utility::getIdealThreadCount()))
		{
			std::shared_ptr<std::thread> thread = std::make_shared<std::thread>(
				[this,
				 &searchTerm,
				 &caseSensitive,
				 &codec,
				 /*no ref here!*/ fileResults,
				 &collection,
				 &collectionMutex]() {
					const int termLength = static_cast<int>(searchTerm.length());
					for (const FullTextSearchResult& fileResult: fileResults)
					{
						const FilePath filePath = getFileNodePath(fileResult.fileId);
						std::shared_ptr<TextAccess> fileContent = getFileContent(filePath, false);

						int charsTotal = 0;
						int lineNumber = 1;
						std::wstring line = codec.decode(fileContent->getLine(lineNumber));

						for (int pos: fileResult.positions)
						{
							while (charsTotal + (int)line.length() <= pos)
							{
								charsTotal += static_cast<int>(line.length());
								lineNumber++;
								line = codec.decode(fileContent->getLine(lineNumber));
							}

							ParseLocation location;
							location.startLineNumber = lineNumber;
							location.startColumnNumber = pos - charsTotal + 1;

							if (caseSensitive &&
								line.substr(location.startColumnNumber - 1, termLength) != searchTerm)
							{
								continue;
							}
							while ((charsTotal + (int)line.length()) < pos + termLength)
							{
								charsTotal += static_cast<int>(line.length());
								lineNumber++;
								line = codec.decode(fileContent->getLine(lineNumber));
							}
							location.endLineNumber = lineNumber;
							location.endColumnNumber = pos + termLength - charsTotal;

							{
								std::lock_guard<std::mutex> lock(collectionMutex);
								// Set first bit to 1 to avoid collisions
								const Id locationId = ~(~Id(0) >> 1) +
									collection->getSourceLocationCount() + 1;
								collection->addSourceLocation(
									LOCATION_FULLTEXT_SEARCH,
									locationId,
									std::vector<Id>(),
									filePath,
									location.startLineNumber,
									location.startColumnNumber,
									location.endLineNumber,
									location.endColumnNumber);
							}
						}
					}
				});
			threads.push_back(thread);
		}

		for (std::shared_ptr<std::thread> thread: threads)
		{
			thread->join();
		}
	}

	addCompleteFlagsToSourceLocationCollection(collection.get());

	MessageStatus(
		std::to_wstring(collection->getSourceLocationCount()) + L" results in " +
			std::to_wstring(collection->getSourceLocationFileCount()) +
			L" files for fulltext search (case-" + (caseSensitive ? L"sensitive" : L"insensitive") +
			L"): " + searchTerm,
		false,
		false)
		.dispatch();

	return collection;
}

std::vector<SearchMatch> PersistentStorage::getAutocompletionMatches(
	const std::wstring& query, NodeTypeSet acceptedNodeTypes, bool acceptCommands) const
{
	TRACE();

	// search in indices
	const size_t maxResultsCount = static_cast<size_t>(std::pow(3, query.size() + 3));
	const size_t maxBestScoredResultsLength = 100;
	const size_t maxMatchesReturned = 1000;

	// create SearchMatches
	std::vector<SearchMatch> matches;

	if (!acceptedNodeTypes
			 .getWithMatchingRemoved([](const NodeType& type) { return type.isFile(); })
			 .isEmpty())
	{
		matches = getAutocompletionSymbolMatches(
			query, acceptedNodeTypes, maxResultsCount, maxBestScoredResultsLength);
	}

	if (acceptedNodeTypes.containsMatching([](const NodeType& type) { return type.isFile(); }))
	{
		utility::append(matches, getAutocompletionFileMatches(query, maxResultsCount));
	}

	if (acceptCommands)
	{
		utility::append(matches, getAutocompletionCommandMatches(query, acceptedNodeTypes));
	}

	// Rescore search matches to check if better score is achieved with higher indices
	std::vector<SearchMatch> rescoredMatches;
	rescoredMatches.reserve(matches.size());

	for (SearchMatch match: matches)
	{
		// rescore match
		if (!match.subtext.empty() && match.indices.size())
		{
			SearchResult newResult = SearchIndex::rescoreText(
				match.name, match.text, match.indices, match.score, maxBestScoredResultsLength);

			match.score = newResult.score;
			match.indices = std::move(newResult.indices);
		}

		rescoredMatches.emplace_back(match);
	}

	// Score child symbol matches with same score as parent lower
	const SearchMatch* lastMatch = nullptr;
	std::set<SearchMatch> matchesSet;

	for (SearchMatch match: rescoredMatches)
	{
		if (lastMatch == nullptr || !utility::isPrefix(lastMatch->name, match.name))
		{
			lastMatch = &match;
		}
		else if (lastMatch->score == match.score && match.tokenNames.size())
		{
			size_t lastSize = lastMatch->name.size();
			if (match.name.find(match.tokenNames[0].getDelimiter(), lastSize) == lastSize)
			{
				match.score -= 10;
			}
			else
			{
				lastMatch = &match;
			}
		}

		matchesSet.insert(match);
	}

	if (matchesSet.size() > maxMatchesReturned)
	{
		auto it = matchesSet.begin();
		std::advance(it, maxMatchesReturned);
		matches = std::vector<SearchMatch>(matchesSet.begin(), it);
	}
	else
	{
		matches = utility::toVector(matchesSet);
	}

	// for (auto a : matches)
	// {
	// 	std::wcout << a.score << " " << a.name << std::endl;
	// }

	return matches;
}

std::vector<SearchMatch> PersistentStorage::getAutocompletionSymbolMatches(
	const std::wstring& query,
	const NodeTypeSet& acceptedNodeTypes,
	size_t maxResultsCount,
	size_t maxBestScoredResultsLength) const
{
	// search in indices
	const std::vector<SearchResult> results = m_symbolIndex.search(
		query, acceptedNodeTypes, maxResultsCount, maxBestScoredResultsLength);

	// fetch StorageNodes for node ids
	std::map<Id, StorageNode> storageNodeMap;
	{
		std::vector<Id> elementIds;

		for (const SearchResult& result: results)
		{
			elementIds.insert(elementIds.end(), result.elementIds.begin(), result.elementIds.end());
		}

		for (const StorageNode& node: m_sqliteIndexStorage.getAllByIds<StorageNode>(elementIds))
		{
			storageNodeMap.emplace(node.id, node);
		}
	}

	// create SearchMatches
	std::vector<SearchMatch> matches;
	for (const SearchResult& result: results)
	{
		SearchMatch match;
		const StorageNode* firstNode = nullptr;

		for (const Id& elementId: result.elementIds)
		{
			if (elementId != 0)
			{
				StorageNode* node = &storageNodeMap[elementId];

				match.tokenIds.push_back(elementId);
				match.tokenNames.push_back(NameHierarchy::deserialize(node->serializedName));

				if (!match.hasChildren &&
					acceptedNodeTypes ==
						NodeTypeSet::all())	   // TODO: check if node types of children match
				{
					match.hasChildren = m_hierarchyCache.nodeHasChildren(elementId);
				}

				if (!firstNode)
				{
					firstNode = node;
				}
			}
		}

		if (!firstNode)
		{
			continue;
		}

		match.name = result.text;
		match.text = result.text;

		NameHierarchy name = NameHierarchy::deserialize(firstNode->serializedName);
		if (name.getQualifiedName() == match.name)
		{
			const size_t idx = m_hierarchyCache.getIndexOfLastVisibleParentNode(firstNode->id);
			if (idx != 0)
			{
				match.text = name.getRange(idx, name.size()).getQualifiedName();
				match.subtext = name.getRange(0, idx).getQualifiedName();
			}
		}

		match.indices = result.indices;
		match.score = result.score;
		match.nodeType = NodeType(intToNodeKind(firstNode->type));
		match.typeName = match.nodeType.getReadableTypeWString();
		match.searchType = SearchMatch::SEARCH_TOKEN;

		if (m_symbolDefinitionKinds.find(firstNode->id) == m_symbolDefinitionKinds.end())
		{
			match.typeName = L"non-indexed " + match.typeName;
		}

		matches.push_back(match);
	}

	return matches;
}

std::vector<SearchMatch> PersistentStorage::getAutocompletionFileMatches(
	const std::wstring& query, size_t maxResultsCount) const
{
	const std::vector<SearchResult> results = m_fileIndex.search(
		query,
		NodeTypeSet::all().getWithMatchingKept([](const NodeType& type) { return type.isFile(); }),
		maxResultsCount,
		100);

	// create SearchMatches
	std::vector<SearchMatch> matches;
	for (const SearchResult& result: results)
	{
		SearchMatch match;

		match.name = result.text;

		match.text = FilePath(match.name).fileName();
		match.subtext = match.name;

		match.tokenIds = result.elementIds;
		for (Id tokenId: match.tokenIds)
		{
			match.tokenNames.push_back(
				NameHierarchy(getFileNodePath(tokenId).wstr(), NAME_DELIMITER_FILE));
		}

		if (!match.tokenNames.size())
		{
			match.tokenNames.push_back(NameHierarchy(match.name, NAME_DELIMITER_FILE));
		}

		match.indices = result.indices;
		match.score = result.score;

		match.nodeType = NodeType(NODE_FILE);
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
	for (const SearchResult& result: results)
	{
		SearchMatch match;

		match.name = result.text;
		match.text = result.text;

		match.indices = result.indices;
		match.score = result.score;

		match.searchType = SearchMatch::SEARCH_COMMAND;
		match.typeName = L"command";

		if (match.getCommandType() == SearchMatch::COMMAND_NODE_FILTER)
		{
			match.nodeType = NodeType(getNodeKindForReadableNodeKindString(match.name));
			match.typeName = L"filter";
		}

		if (acceptedNodeTypes == NodeTypeSet::all() ||
			(match.getCommandType() == SearchMatch::COMMAND_NODE_FILTER &&
			 !(acceptedNodeTypes.contains(match.nodeType))))
		{
			matches.push_back(match);
		}
	}

	return matches;
}

std::vector<SearchMatch> PersistentStorage::getSearchMatchesForTokenIds(
	const std::vector<Id>& elementIds) const
{
	TRACE();

	// todo: what if all these elements share the same node in the searchindex?
	// In that case there should be only one search match.
	std::vector<SearchMatch> matches;

	// fetch StorageNodes for node ids
	std::map<Id, StorageNode> storageNodeMap;
	for (StorageNode& node: m_sqliteIndexStorage.getAllByIds<StorageNode>(elementIds))
	{
		storageNodeMap.emplace(node.id, node);
	}

	for (Id elementId: elementIds)
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
		match.tokenNames.push_back(nameHierarchy);
		match.nodeType = NodeType(intToNodeKind(node.type));
		match.searchType = SearchMatch::SEARCH_TOKEN;

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
	const size_t sdk_size = m_symbolDefinitionKinds.size();
	m_sqliteIndexStorage.forEach<StorageNode>([&, sdk_size](StorageNode&& storageNode) {
		const NodeType type(intToNodeKind(storageNode.type));
		if (type.isFile())
		{
			auto fn_it = m_fileNodeIndexed.find(storageNode.id);
			if (fn_it != m_fileNodeIndexed.end() && fn_it->second)
			{
				addFileNodeToGraph(storageNode, graph.get());
			}
		}
		else
		{
			bool showNode = true;
			if (sdk_size)
			{
				auto it = m_symbolDefinitionKinds.find(storageNode.id);
				showNode = (it != m_symbolDefinitionKinds.end() && it->second == DEFINITION_EXPLICIT);
			}
			if (showNode &&
				(type.isPackage() ||
				 !m_hierarchyCache.isChildOfVisibleNodeOrInvisible(storageNode.id)))
			{
				addNodeToGraph(storageNode, type, graph.get(), false);
			}
		}
	});
	return graph;
}

std::shared_ptr<Graph> PersistentStorage::getGraphForNodeTypes(NodeTypeSet nodeTypes) const
{
	TRACE();

	std::vector<Id> tokenIds;

	m_sqliteIndexStorage.forEach<StorageNode>([&](StorageNode&& node) {
		if (nodeTypes.contains(NodeType(intToNodeKind(node.type))))
		{
			auto it = m_symbolDefinitionKinds.find(node.id);
			if (it != m_symbolDefinitionKinds.end() && it->second == DEFINITION_EXPLICIT)
			{
				tokenIds.push_back(node.id);
			}
		}
	});

	if (nodeTypes.containsMatching([](const NodeType& type) { return type.isFile(); }))
	{
		for (const auto& p: m_fileNodePaths)
		{
			tokenIds.push_back(p.first);
		}
	}

	std::shared_ptr<Graph> graph = std::make_shared<Graph>();
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
			const NodeType nodeType(intToNodeKind(node.type));
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

				// don't expand active node if it has too many child nodes
				if (nodeIds.size() > 100 && nodeType.isCollapsible())
				{
					nodeIds.clear();
				}

				nodeIds.push_back(elementId);
				edgeIds.clear();

				for (const StorageEdge& edge:
					 m_sqliteIndexStorage.getEdgesBySourceOrTargetId(elementId))
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
		for (const StorageSymbol& symbol: m_sqliteIndexStorage.getAllByIds<StorageSymbol>(ids))
		{
			if (symbol.id > 0 &&
				(!isPackage || intToDefinitionKind(symbol.definitionKind) != DEFINITION_IMPLICIT))
			{
				nodeIds.push_back(symbol.id);
			}
			symbolIds.insert(symbol.id);
		}
		for (const StorageNode& node: m_sqliteIndexStorage.getAllByIds<StorageNode>(ids))
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
				for (const StorageEdge& edge: m_sqliteIndexStorage.getAllByIds<StorageEdge>(ids))
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

		for (Id nodeId: expandedNodeIds)
		{
			if (graph->getNodeById(nodeId))
			{
				m_hierarchyCache.addFirstChildIdsForNodeId(
					nodeId, &expandedChildIds, &expandedChildEdgeIds);
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
	addComponentIsAmbiguousToGraph(graph);

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
	Id originId,
	Id targetId,
	NodeKindMask nodeTypes,
	Edge::TypeMask edgeTypes,
	bool nodeNonIndexed,
	size_t depth,
	bool directed) const
{
	TRACE();

	std::set<Id> nodeIds;
	std::set<Id> edgeIds;

	nodeIds.insert(originId ? originId : targetId);
	bool forward = originId;
	size_t currentDepth = 0;

	std::vector<Id> nodeIdsToProcess = {*nodeIds.begin()};

	struct TrailNode
	{
		Id id = 0;
		std::set<TrailNode*> parents;
		std::set<Id> edgeIds;
	};

	bool isTerminatedTrail = originId && targetId;
	std::map<Id, TrailNode> trailNodes;

	if (isTerminatedTrail)
	{
		TrailNode root;
		root.id = originId;
		trailNodes.emplace(originId, root);
	}

	while (nodeIdsToProcess.size() && (!depth || currentDepth < depth))
	{
		std::vector<StorageEdge> edges = forward
			? m_sqliteIndexStorage.getEdgesBySourceIds(nodeIdsToProcess)
			: m_sqliteIndexStorage.getEdgesByTargetIds(nodeIdsToProcess);

		if (!directed || edgeTypes & Edge::LAYOUT_VERTICAL)
		{
			utility::append(
				edges,
				forward ? m_sqliteIndexStorage.getEdgesByTargetIds(nodeIdsToProcess)
						: m_sqliteIndexStorage.getEdgesBySourceIds(nodeIdsToProcess));
		}

		std::vector<Id> nodeIdsToCheck;
		std::map<Id, std::vector<StorageEdge>> edgesToInsert;

		for (const StorageEdge& edge: edges)
		{
			if (Edge::intToType(edge.type) & edgeTypes && edgeIds.find(edge.id) == edgeIds.end())
			{
				bool isForward = forward == !(Edge::intToType(edge.type) & Edge::LAYOUT_VERTICAL);

				const Id targetNodeId = isForward ? edge.targetNodeId : edge.sourceNodeId;
				const Id sourceNodeId = isForward ? edge.sourceNodeId : edge.targetNodeId;

				if (nodeIds.find(targetNodeId) == nodeIds.end())
				{
					nodeIdsToCheck.push_back(targetNodeId);
					edgesToInsert[targetNodeId].push_back(edge);
				}
				else if (nodeIds.find(sourceNodeId) == nodeIds.end())
				{
					if (!directed)
					{
						nodeIdsToCheck.push_back(sourceNodeId);
						edgesToInsert[sourceNodeId].push_back(edge);
					}
				}
				else
				{
					edgeIds.insert(edge.id);

					if (isTerminatedTrail)
					{
						TrailNode& target = trailNodes[targetNodeId];
						TrailNode& origin = trailNodes[sourceNodeId];
						target.id = targetNodeId;
						origin.id = sourceNodeId;
						target.parents.insert(&origin);
						target.edgeIds.insert(edge.id);
					}
				}
			}
		}

		nodeIdsToProcess.clear();

		if (nodeTypes != 0)
		{
			for (const StorageNode& node:
				 m_sqliteIndexStorage.getAllByIds<StorageNode>(nodeIdsToCheck))
			{
				NodeKind kind = intToNodeKind(node.type);
				if (kind & nodeTypes || (kind == NODE_SYMBOL && nodeNonIndexed))
				{
					if (!nodeNonIndexed)
					{
						if (kind == NODE_FILE)
						{
							auto it = m_fileNodeIndexed.find(node.id);
							if (it == m_fileNodeIndexed.end() || !it->second)
							{
								continue;
							}
						}
						else
						{
							auto it = m_symbolDefinitionKinds.find(node.id);
							if (it == m_symbolDefinitionKinds.end() || it->second == DEFINITION_NONE)
							{
								continue;
							}
						}
					}

					// FIXME: don't add namespace nodes to the graph, because it destroys trail
					// layouting Remove when namespaces are proper nodes with children
					if ((kind & (NODE_MODULE | NODE_NAMESPACE | NODE_PACKAGE)) == 0)
					{
						nodeIds.insert(node.id);
						for (const StorageEdge& edge: edgesToInsert[node.id])
						{
							if ((Edge::intToType(edge.type) & Edge::EDGE_MEMBER) == 0)
							{
								edgeIds.insert(edge.id);
							}
						}
					}
					nodeIdsToProcess.push_back(node.id);

					if (isTerminatedTrail)
					{
						TrailNode& targetNode = trailNodes[node.id];
						targetNode.id = node.id;

						for (const StorageEdge& edge: edgesToInsert[node.id])
						{
							targetNode.edgeIds.insert(edge.id);

							Id sourceNodeId =
								(edge.targetNodeId == node.id ? edge.sourceNodeId
															  : edge.targetNodeId);
							TrailNode& oldNode = trailNodes[sourceNodeId];
							targetNode.parents.insert(&oldNode);
						}
					}
				}
			}
		}
		else
		{
			for (const Id nodeId: nodeIdsToCheck)
			{
				nodeIds.insert(nodeId);
				nodeIdsToProcess.push_back(nodeId);

				for (const StorageEdge& edge: edgesToInsert[nodeId])
				{
					edgeIds.insert(edge.id);
				}
			}
		}

		edgesToInsert.clear();

		currentDepth++;
	}

	if (isTerminatedTrail)
	{
		nodeIds.clear();
		edgeIds.clear();

		if (trailNodes.find(targetId) != trailNodes.end())
		{
			std::queue<TrailNode*> nodesToProcess;
			nodesToProcess.push(&trailNodes[targetId]);

			while (nodesToProcess.size())
			{
				TrailNode* currentNode = nodesToProcess.front();
				nodesToProcess.pop();

				if (nodeIds.find(currentNode->id) != nodeIds.end())
				{
					continue;
				}

				nodeIds.insert(currentNode->id);
				edgeIds.insert(currentNode->edgeIds.begin(), currentNode->edgeIds.end());

				for (TrailNode* parent: currentNode->parents)
				{
					nodesToProcess.push(parent);
				}
			}
		}
		else
		{
			nodeIds.insert(originId);
		}
	}

	std::shared_ptr<Graph> graph = std::make_shared<Graph>();

	addNodesWithParentsAndEdgesToGraph(
		utility::toVector(nodeIds), utility::toVector(edgeIds), graph.get(), false);
	addComponentAccessToGraph(graph.get());
	addComponentIsAmbiguousToGraph(graph.get());

	return graph;
}

NodeKindMask PersistentStorage::getAvailableNodeTypes() const
{
	TRACE();

	NodeKindMask mask = 0;
	for (int type: m_sqliteIndexStorage.getAvailableNodeTypes())
	{
		mask |= intToNodeKind(type);
	}
	return mask;
}

Edge::TypeMask PersistentStorage::getAvailableEdgeTypes() const
{
	TRACE();

	Edge::TypeMask mask = 0;
	for (int type: m_sqliteIndexStorage.getAvailableEdgeTypes())
	{
		mask |= Edge::intToType(type);
	}
	return mask;
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

		for (const StorageEdge& edge: m_sqliteIndexStorage.getEdgesByTargetId(tokenId))
		{
			activeTokenIds.push_back(edge.id);
		}
	}

	return activeTokenIds;
}

std::vector<Id> PersistentStorage::getNodeIdsForLocationIds(const std::vector<Id>& locationIds) const
{
	TRACE();

	std::set<Id> nodeIds;
	std::set<Id> implicitNodeIds;

	for (const StorageOccurrence& occurrence:
		 m_sqliteIndexStorage.getOccurrencesForLocationIds(locationIds))
	{
		Id elementId = occurrence.elementId;

		const StorageEdge edge = m_sqliteIndexStorage.getFirstById<StorageEdge>(elementId);
		if (edge.id != 0)
		{
			elementId = edge.targetNodeId;
		}

		auto it = m_symbolDefinitionKinds.find(elementId);
		if ((it != m_symbolDefinitionKinds.end() && it->second == DEFINITION_IMPLICIT) ||
			Edge::intToType(edge.type) == Edge::EDGE_OVERRIDE)
		{
			implicitNodeIds.insert(elementId);
		}
		else
		{
			nodeIds.insert(elementId);
		}
	}

	if (nodeIds.size())
	{
		return utility::toVector(nodeIds);
	}

	return utility::toVector(implicitNodeIds);
}

std::shared_ptr<SourceLocationCollection> PersistentStorage::getSourceLocationsForTokenIds(
	const std::vector<Id>& tokenIds) const
{
	TRACE();

	std::map<Id, FilePath> filePaths;
	std::vector<Id> nonFileIds;

	for (const Id tokenId: tokenIds)
	{
		FilePath path = getFileNodePath(tokenId);

		// check for non-indexed file
		if (path.empty() && m_symbolDefinitionKinds.find(tokenId) == m_symbolDefinitionKinds.end())
		{
			const StorageNode fileNode = m_sqliteIndexStorage.getNodeById(tokenId);
			if (NodeType(intToNodeKind(fileNode.type)).isFile())
			{
				path = FilePath(
					NameHierarchy::deserialize(fileNode.serializedName).getQualifiedName());
			}
		}

		if (path.empty())
		{
			nonFileIds.push_back(tokenId);
		}
		else
		{
			filePaths.emplace(tokenId, path);
		}
	}

	std::shared_ptr<SourceLocationCollection> collection =
		std::make_shared<SourceLocationCollection>();
	for (const std::pair<Id, FilePath>& p: filePaths)
	{
		collection->addSourceLocationFile(std::make_shared<SourceLocationFile>(
			p.second, getFileNodeLanguage(p.first), true, false, false));
	}

	if (nonFileIds.size())
	{
		// FIXME: can we use get SqliteIndexStorage::getSourceLocationsForElementIds() here instead?
		std::vector<Id> locationIds;
		std::unordered_map<Id, Id> locationIdToElementIdMap;
		for (const StorageOccurrence& occurrence:
			 m_sqliteIndexStorage.getOccurrencesForElementIds(nonFileIds))
		{
			locationIds.push_back(occurrence.sourceLocationId);
			locationIdToElementIdMap[occurrence.sourceLocationId] = occurrence.elementId;
		}

		for (const StorageSourceLocation& sourceLocation:
			 m_sqliteIndexStorage.getAllByIds<StorageSourceLocation>(locationIds))
		{
			const LocationType type = intToLocationType(sourceLocation.type);
			if (type != LOCATION_TOKEN && type != LOCATION_SCOPE && type != LOCATION_LOCAL_SYMBOL &&
				type != LOCATION_UNSOLVED)
			{
				continue;
			}

			auto it = locationIdToElementIdMap.find(sourceLocation.id);
			if (it == locationIdToElementIdMap.end())
			{
				continue;
			}

			FilePath path = getFileNodePath(sourceLocation.fileNodeId);
			// FIXME: This shouldn't be necessary since all files are stored, even non-indexed
			if (path.empty())
			{
				const StorageNode fileNode = m_sqliteIndexStorage.getNodeById(
					sourceLocation.fileNodeId);
				if (fileNode.id)
				{
					const FilePath path2 = FilePath(
						NameHierarchy::deserialize(fileNode.serializedName).getQualifiedName());
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
					{it->second},
					path,
					sourceLocation.startLine,
					sourceLocation.startCol,
					sourceLocation.endLine,
					sourceLocation.endCol);
			}
		}
	}

	addCompleteFlagsToSourceLocationCollection(collection.get());

	return collection;
}

std::shared_ptr<SourceLocationCollection> PersistentStorage::getSourceLocationsForLocationIds(
	const std::vector<Id>& locationIds) const
{
	TRACE();

	std::shared_ptr<SourceLocationCollection> collection =
		std::make_shared<SourceLocationCollection>();

	std::map<Id, std::vector<Id>> m_locationIdToElementIds;
	for (const StorageOccurrence& occurrence:
		 m_sqliteIndexStorage.getOccurrencesForLocationIds(locationIds))
	{
		m_locationIdToElementIds[occurrence.sourceLocationId].push_back(occurrence.elementId);
	}

	for (StorageSourceLocation location:
		 m_sqliteIndexStorage.getAllByIds<StorageSourceLocation>(locationIds))
	{
		const LocationType type = intToLocationType(location.type);
		if (type != LOCATION_TOKEN && type != LOCATION_SCOPE && type != LOCATION_LOCAL_SYMBOL &&
			type != LOCATION_UNSOLVED)
		{
			continue;
		}

		collection->addSourceLocation(
			intToLocationType(location.type),
			location.id,
			m_locationIdToElementIds[location.id],
			getFileNodePath(location.fileNodeId),
			location.startLine,
			location.startCol,
			location.endLine,
			location.endCol);
	}

	addCompleteFlagsToSourceLocationCollection(collection.get());

	return collection;
}

std::shared_ptr<SourceLocationFile> PersistentStorage::getSourceLocationsForFile(
	const FilePath& filePath) const
{
	TRACE();

	return m_sqliteIndexStorage.getSourceLocationsForFile(filePath)->getFilteredByTypes(
		{LOCATION_TOKEN, LOCATION_SCOPE, LOCATION_QUALIFIER, LOCATION_LOCAL_SYMBOL, LOCATION_UNSOLVED});
}

std::shared_ptr<SourceLocationFile> PersistentStorage::getSourceLocationsForLinesInFile(
	const FilePath& filePath, size_t startLine, size_t endLine) const
{
	TRACE();

	return m_sqliteIndexStorage.getSourceLocationsForLinesInFile(filePath, startLine, endLine)
		->getFilteredByLines(startLine, endLine)
		->getFilteredByTypes(
			{LOCATION_TOKEN,
			 LOCATION_SCOPE,
			 LOCATION_QUALIFIER,
			 LOCATION_LOCAL_SYMBOL,
			 LOCATION_UNSOLVED});
}

std::shared_ptr<SourceLocationFile> PersistentStorage::getSourceLocationsOfTypeInFile(
	const FilePath& filePath, LocationType type) const
{
	TRACE();

	return m_sqliteIndexStorage.getSourceLocationsOfTypeInFile(filePath, type);
}

std::shared_ptr<TextAccess> PersistentStorage::getFileContent(
	const FilePath& filePath, bool showsErrors) const
{
	TRACE();

	std::shared_ptr<TextAccess> fileContent = m_sqliteIndexStorage.getFileContentByPath(
		filePath.wstr());
	if (fileContent->getLineCount() > 0)
	{
		return fileContent;
	}
	return TextAccess::createFromFile(FilePath(filePath));
}

bool PersistentStorage::hasContentForFile(const FilePath& filePath) const
{
	std::shared_ptr<TextAccess> fileContent = m_sqliteIndexStorage.getFileContentByPath(
		filePath.wstr());
	if (fileContent->getLineCount() > 0)
	{
		return true;
	}
	return false;
}

FileInfo PersistentStorage::getFileInfoForFileId(Id id) const
{
	StorageFile storageFile = m_sqliteIndexStorage.getFirstById<StorageFile>(id);
	return FileInfo(FilePath(storageFile.filePath), storageFile.modificationTime);
}

FileInfo PersistentStorage::getFileInfoForFilePath(const FilePath& filePath) const
{
	return getFileInfoForFileId(getFileNodeId(filePath));
}

std::vector<FileInfo> PersistentStorage::getFileInfosForFilePaths(
	const std::vector<FilePath>& filePaths) const
{
	std::vector<FileInfo> fileInfos;

	for (const StorageFile& file: m_sqliteIndexStorage.getFilesByPaths(filePaths))
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
	return ErrorCountInfo(m_sqliteIndexStorage.getAllErrorInfos());
}

std::vector<ErrorInfo> PersistentStorage::getErrorsLimited(const ErrorFilter& filter) const
{
	return filter.filterErrors(m_sqliteIndexStorage.getAllErrorInfos());
}

std::vector<ErrorInfo> PersistentStorage::getErrorsForFileLimited(
	const ErrorFilter& filter, const FilePath& filePath) const
{
	Id fileId = getFileNodeId(filePath);
	std::set<Id> fileIds = {fileId};

	std::unordered_map<Id, std::set<Id>> includedMap = getFileIdToIncludedFileIdMap();
	std::set<Id> fileIdsToProcess = includedMap[getFileNodeId(filePath)];
	std::set<Id> processedFileIds;

	while (fileIdsToProcess.size())
	{
		std::set<Id> nextFileIdsToProcess;
		for (Id id: fileIdsToProcess)
		{
			if (fileIds.insert(id).second)
			{
				utility::append(nextFileIdsToProcess, includedMap[id]);
			}
		}
		fileIdsToProcess = nextFileIdsToProcess;
	}

	std::vector<ErrorInfo> res;

	std::vector<ErrorInfo> errors = m_sqliteIndexStorage.getAllErrorInfos();
	for (const ErrorInfo& error: errors)
	{
		if (filter.filter(error) &&
			fileIds.find(getFileNodeId(FilePath(error.filePath))) != fileIds.end())
		{
			res.push_back(error);
		}
	}

	if (res.empty())
	{
		std::unordered_map<Id, std::set<Id>> includingMap = getFileIdToIncludingFileIdMap();
		fileIds.clear();

		fileIdsToProcess = includingMap[fileId];
		while (fileIdsToProcess.size())
		{
			std::set<Id> nextFileIdsToProcess;
			for (Id id: fileIdsToProcess)
			{
				if (fileIds.insert(id).second)
				{
					utility::append(nextFileIdsToProcess, includingMap[id]);
				}
			}
			fileIdsToProcess = nextFileIdsToProcess;
		}

		for (const ErrorInfo& error: errors)
		{
			if (error.fatal && filter.filter(error) &&
				fileIds.find(getFileNodeId(FilePath(error.filePath))) != fileIds.end())
			{
				res.push_back(error);
			}
		}
	}

	return res;
}

std::shared_ptr<SourceLocationCollection> PersistentStorage::getErrorSourceLocations(
	const std::vector<ErrorInfo>& errors) const
{
	TRACE();

	std::shared_ptr<SourceLocationCollection> collection =
		std::make_shared<SourceLocationCollection>();

	for (const ErrorInfo& error: errors)
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
			error.columnNumber);
	}

	addCompleteFlagsToSourceLocationCollection(collection.get());

	return collection;
}

Id PersistentStorage::addNodeBookmark(const NodeBookmark& bookmark)
{
	const Id categoryId = addBookmarkCategory(bookmark.getCategory().getName());
	const Id id = m_sqliteBookmarkStorage
					  .addBookmark(StorageBookmarkData(
						  bookmark.getName(),
						  bookmark.getComment(),
						  bookmark.getTimeStamp().toString(),
						  categoryId))
					  .id;

	for (const Id& nodeId: bookmark.getNodeIds())
	{
		m_sqliteBookmarkStorage.addBookmarkedNode(
			StorageBookmarkedNodeData(id, m_sqliteIndexStorage.getNodeById(nodeId).serializedName));
	}

	return id;
}

Id PersistentStorage::addEdgeBookmark(const EdgeBookmark& bookmark)
{
	const Id categoryId = addBookmarkCategory(bookmark.getCategory().getName());
	const Id id = m_sqliteBookmarkStorage
					  .addBookmark(StorageBookmarkData(
						  bookmark.getName(),
						  bookmark.getComment(),
						  bookmark.getTimeStamp().toString(),
						  categoryId))
					  .id;
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
			sourceNodeActive));
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
	const Id bookmarkId,
	const std::wstring& name,
	const std::wstring& comment,
	const std::wstring& categoryName)
{
	const Id categoryId = addBookmarkCategory(
		categoryName);	  // only creates category if id didn't exist before;
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
	for (const StorageBookmarkCategory& bookmarkCategory:
		 m_sqliteBookmarkStorage.getAllBookmarkCategories())
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
		if (itCategories != bookmarkCategories.end() &&
			itNodeIds != bookmarkIdToBookmarkedNodeIds.end())
		{
			NodeBookmark bookmark(
				storageBookmark.id,
				storageBookmark.name,
				storageBookmark.comment,
				storageBookmark.timestamp,
				BookmarkCategory(itCategories->second.id, itCategories->second.name));
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
	for (const StorageBookmarkCategory& bookmarkCategory:
		 m_sqliteBookmarkStorage.getAllBookmarkCategories())
	{
		bookmarkCategories[bookmarkCategory.id] = bookmarkCategory;
	}

	std::unordered_map<Id, std::vector<StorageBookmarkedEdge>> bookmarkIdToBookmarkedEdges;
	for (const StorageBookmarkedEdge& bookmarkedEdge: m_sqliteBookmarkStorage.getAllBookmarkedEdges())
	{
		bookmarkIdToBookmarkedEdges[bookmarkedEdge.bookmarkId].push_back(bookmarkedEdge);
	}

	std::vector<EdgeBookmark> edgeBookmarks;

	UnorderedCache<std::wstring, Id> nodeIdCache([&](const std::wstring& serializedNodeName) {
		return m_sqliteIndexStorage.getNodeBySerializedName(serializedNodeName).id;
	});

	for (const StorageBookmark& storageBookmark: m_sqliteBookmarkStorage.getAllBookmarks())
	{
		auto itCategories = bookmarkCategories.find(storageBookmark.categoryId);
		auto itBookmarkedEdges = bookmarkIdToBookmarkedEdges.find(storageBookmark.id);
		if (itCategories != bookmarkCategories.end() &&
			itBookmarkedEdges != bookmarkIdToBookmarkedEdges.end())
		{
			EdgeBookmark bookmark(
				storageBookmark.id,
				storageBookmark.name,
				storageBookmark.comment,
				storageBookmark.timestamp,
				BookmarkCategory(itCategories->second.id, itCategories->second.name));

			Id activeNodeId = 0;
			for (const StorageBookmarkedEdge& bookmarkedEdge: itBookmarkedEdges->second)
			{
				const Id sourceNodeId = nodeIdCache.getValue(bookmarkedEdge.serializedSourceNodeName);
				const Id targetNodeId = nodeIdCache.getValue(bookmarkedEdge.serializedTargetNodeName);
				const Id edgeId = m_sqliteIndexStorage
									  .getEdgeBySourceTargetType(
										  sourceNodeId, targetNodeId, bookmarkedEdge.edgeType)
									  .id;
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
	for (const StorageBookmarkCategory& storageBookmarkCategoriy:
		 m_sqliteBookmarkStorage.getAllBookmarkCategories())
	{
		categories.push_back(
			BookmarkCategory(storageBookmarkCategoriy.id, storageBookmarkCategoriy.name));
	}
	return categories;
}

TooltipInfo PersistentStorage::getTooltipInfoForTokenIds(
	const std::vector<Id>& tokenIds, TooltipOrigin origin) const
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

	const NodeType type(intToNodeKind(node.type));
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

	if (type.isFile())
	{
		if (!getFileNodeIndexed(node.id))
		{
			info.title = L"non-indexed " + info.title;
		}

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
	for (const auto& edge: m_sqliteIndexStorage.getEdgesByTargetId(node.id))
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
		FilePath(L"main.txt"), L"", true, true, true);

	// set file language
	std::vector<StorageOccurrence> occurrences = m_sqliteIndexStorage.getOccurrencesForElementIds(
		{node.id});
	if (occurrences.size())
	{
		const Id locationId = occurrences.front().sourceLocationId;
		const Id fileId =
			m_sqliteIndexStorage.getFirstById<StorageSourceLocation>(locationId).fileNodeId;
		snippet.locationFile->setLanguage(getFileNodeLanguage(fileId));
	}

	if (nameHierarchy.hasSignature())
	{
		std::shared_ptr<SourceLocationCollection> locations =
			m_sqliteIndexStorage.getSourceLocationsForElementIds({node.id});
		SourceLocation* sigLoc = nullptr;

		locations->forEachSourceLocation([&sigLoc](SourceLocation* location) {
			if (!sigLoc && location->isStartLocation() && location->getType() == LOCATION_SIGNATURE)
			{
				sigLoc = location;
			}
		});

		// if node has a signature location use that one
		// use while for breaking out
		while (sigLoc)
		{
			struct Annotation
			{
				Id locationId = 0;
				size_t startPos = 0;
				size_t endPos = 0;
			};

			std::vector<Annotation> annotations;
			std::vector<std::string> lines =
				getFileContent(sigLoc->getFilePath(), false)
					->getLines(
						static_cast<unsigned int>(sigLoc->getLineNumber()),
						static_cast<unsigned int>(sigLoc->getEndLocation()->getLineNumber()));

			// check if signature location refers to correct locations in the code
			// wrongly recorded signature locations of implicit template methods in C++ caused crashes
			if (lines.empty() ||
				lines.size() !=
					sigLoc->getOtherLocation()->getLineNumber() - sigLoc->getLineNumber() + 1 ||
				sigLoc->getColumnNumber() > lines[0].size() ||
				sigLoc->getOtherLocation()->getColumnNumber() > lines[lines.size() - 1].size())
			{
				break;
			}

			std::shared_ptr<SourceLocationFile> file = getSourceLocationsForLinesInFile(
				sigLoc->getFilePath(),
				sigLoc->getStartLocation()->getLineNumber(),
				sigLoc->getEndLocation()->getLineNumber());

			file->forEachStartSourceLocation([&sigLoc, &annotations, &lines](SourceLocation* loc) {
				if ((loc->getType() == LOCATION_TOKEN || loc->getType() == LOCATION_QUALIFIER) &&
					sigLoc->contains(*loc))
				{
					Annotation annotation;
					annotation.locationId = loc->getLocationId();

					for (size_t i = 0;
						 i < loc->getLineNumber() - sigLoc->getLineNumber() && i < lines.size();
						 i++)
					{
						annotation.startPos += lines[i].size();
					}
					annotation.startPos += loc->getColumnNumber() - sigLoc->getColumnNumber();

					for (size_t i = 0;
						 i < loc->getEndLocation()->getLineNumber() - sigLoc->getLineNumber() &&
						 i < lines.size();
						 i++)
					{
						annotation.endPos += lines[i].size();
					}
					annotation.endPos += loc->getEndLocation()->getColumnNumber() -
						sigLoc->getColumnNumber();

					annotations.push_back(annotation);
				}
			});

			// remove characters after signature end
			lines[lines.size() - 1] = lines[lines.size() - 1].substr(
				0, sigLoc->getEndLocation()->getColumnNumber());

			// remove characters before signature start
			lines[0] = lines[0].substr(sigLoc->getColumnNumber() - 1);

			std::wstring code = utility::decodeFromUtf8(utility::join(lines, ""));

			// store texts of annotations
			std::vector<std::pair<Id, std::wstring>> annotatedTexts;
			std::wstring delimiter = nameHierarchy.getDelimiter();
			size_t offset = 0;
			for (const Annotation& annotation: annotations)
			{
				std::wstring text = code.substr(
					annotation.startPos + offset, annotation.endPos - annotation.startPos + 1);

				// only replace name if not already prepended with other qualifiers
				size_t delimiterPos = code.rfind(delimiter, annotation.startPos);

				// if is function name itself, replace with qualified name
				if (utility::containsElement(
						file->getSourceLocationById(annotation.locationId)->getTokenIds(), node.id) &&
					(delimiterPos == std::wstring::npos ||
					 delimiterPos < annotation.startPos - delimiter.size()) &&
					text.size() <= nameHierarchy.getRawName().size())
				{
					std::wstring name = nameHierarchy.getQualifiedName();
					offset = name.size() - text.size();

					code = code.replace(
						annotation.startPos, annotation.endPos - annotation.startPos + 1, name);
					text = name;
				}
				else
				{
					text = utility::convertWhiteSpacesToSingleSpaces(text);
				}
				annotatedTexts.push_back(std::make_pair(annotation.locationId, text));
			}

			// format
			code = utility::convertWhiteSpacesToSingleSpaces(code);
			snippet.code = utility::breakSignature(
				code, 50, ApplicationSettings::getInstance()->getCodeTabWidth());

			// create source locations for annotations via stored texts
			size_t pos = 0;
			for (const std::pair<Id, std::wstring>& p: annotatedTexts)
			{
				pos = snippet.code.find(p.second, pos);
				if (pos != std::wstring::npos)
				{
					SourceLocation* loc = file->getSourceLocationById(p.first);

					snippet.locationFile->addSourceLocation(
						loc->getType(),
						loc->getLocationId(),
						loc->getTokenIds(),
						1,
						pos + 1,
						1,
						pos + p.second.size());

					pos += p.second.size();
				}
			}

			return snippet;
		}

		// otherwise augment the name with signature with locations for type usages
		snippet.code = utility::breakSignature(
			nameHierarchy.getSignature().getPrefix(),
			nameHierarchy.getQualifiedName(),
			nameHierarchy.getSignature().getPostfix(),
			50,
			ApplicationSettings::getInstance()->getCodeTabWidth());

		std::vector<Id> typeNodeIds;
		for (const auto& edge: m_sqliteIndexStorage.getEdgesBySourceId(node.id))
		{
			if (Edge::intToType(edge.type) == Edge::EDGE_TYPE_USAGE)
			{
				typeNodeIds.push_back(edge.targetNodeId);
			}
		}

		std::set<
			std::pair<std::wstring, Id>,
			bool (*)(const std::pair<std::wstring, Id>&, const std::pair<std::wstring, Id>&)>
			typeNames([](const std::pair<std::wstring, Id>& a, const std::pair<std::wstring, Id>& b) {
				if (a.first.size() == b.first.size())
				{
					return a.first < b.first;
				}

				return a.first.size() > b.first.size();
			});

		typeNames.insert(std::make_pair(nameHierarchy.getQualifiedName(), node.id));
		for (const auto& typeNode: m_sqliteIndexStorage.getAllByIds<StorageNode>(typeNodeIds))
		{
			typeNames.insert(std::make_pair(
				NameHierarchy::deserialize(typeNode.serializedName).getQualifiedName(), typeNode.id));
		}

		std::vector<std::pair<size_t, size_t>> locationRanges;
		for (const auto& p: typeNames)
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
				for (const auto& locationRange: locationRanges)
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
						LOCATION_TOKEN,
						0,
						std::vector<Id>(1, p.second),
						1,
						pos + 1,
						1,
						pos + p.first.size());

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
		std::wstring fileLanguage = getFileNodeLanguage(
			m_sqliteIndexStorage.getFirstById<StorageSourceLocation>(locationIds.front()).fileNodeId);

		const std::vector<Id> nodeIds = getNodeIdsForLocationIds(locationIds);

		for (const StorageNode& node: m_sqliteIndexStorage.getAllByIds<StorageNode>(nodeIds))
		{
			TooltipSnippet snippet;

			const NameHierarchy nameHierarchy = NameHierarchy::deserialize(node.serializedName);
			snippet.code = nameHierarchy.getQualifiedName();
			snippet.locationFile = std::make_shared<SourceLocationFile>(
				FilePath(L"main.txt"), fileLanguage, true, true, true);

			snippet.locationFile->addSourceLocation(
				LOCATION_TOKEN, 0, std::vector<Id>(1, node.id), 1, 1, 1, snippet.code.size());

			if (NodeType(intToNodeKind(node.type)).isCallable())
			{
				snippet.code += L"()";
			}

			info.snippets.push_back(snippet);
		}
	}

	for (Id id: localSymbolIds)
	{
		TooltipSnippet snippet;

		snippet.code = L"local symbol";
		snippet.locationFile = std::make_shared<SourceLocationFile>(
			FilePath(L"main.txt"), L"", true, true, true);
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

	{
		std::map<FilePath, Id>::const_iterator it = m_fileNodeIds.find(filePath);
		if (it != m_fileNodeIds.end())
		{
			return it->second;
		}
	}
	{
		std::map<FilePath, Id>::const_iterator it = m_lowerCasefileNodeIds.find(
			filePath.getLowerCase());
		if (it != m_lowerCasefileNodeIds.end())
		{
			return it->second;
		}
	}

	return 0;
}

std::vector<Id> PersistentStorage::getFileNodeIds(const std::vector<FilePath>& filePaths) const
{
	std::vector<Id> ids;
	for (const FilePath& path: filePaths)
	{
		ids.push_back(getFileNodeId(path));
	}
	return ids;
}

std::set<Id> PersistentStorage::getFileNodeIds(const std::set<FilePath>& filePaths) const
{
	std::set<Id> ids;
	for (const FilePath& path: filePaths)
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

bool PersistentStorage::getFileNodeComplete(Id fileId) const
{
	auto it = m_fileNodeComplete.find(fileId);
	if (it != m_fileNodeComplete.end())
	{
		return it->second;
	}

	return false;
}

bool PersistentStorage::getFileNodeIndexed(Id fileId) const
{
	auto it = m_fileNodeIndexed.find(fileId);
	if (it != m_fileNodeIndexed.end())
	{
		return it->second;
	}

	return false;
}

std::wstring PersistentStorage::getFileNodeLanguage(Id fileId) const
{
	auto it = m_fileNodeLanguage.find(fileId);
	if (it != m_fileNodeLanguage.end())
	{
		return it->second;
	}

	return L"";
}

std::unordered_map<Id, std::set<Id>> PersistentStorage::getFileIdToIncludingFileIdMap() const
{
	std::unordered_map<Id, std::set<Id>> fileIdToIncludingFileIdMap;

	m_sqliteIndexStorage.forEachOfType<StorageEdge>(
		Edge::typeToInt(Edge::EDGE_INCLUDE), [&fileIdToIncludingFileIdMap](StorageEdge&& edge) {
			fileIdToIncludingFileIdMap[edge.targetNodeId].insert(edge.sourceNodeId);
		});

	return fileIdToIncludingFileIdMap;
}

std::unordered_map<Id, std::set<Id>> PersistentStorage::getFileIdToIncludedFileIdMap() const
{
	std::unordered_map<Id, std::set<Id>> fileIdToIncludingFileIdMap;

	m_sqliteIndexStorage.forEachOfType<StorageEdge>(
		Edge::typeToInt(Edge::EDGE_INCLUDE), [&fileIdToIncludingFileIdMap](StorageEdge&& edge) {
			fileIdToIncludingFileIdMap[edge.sourceNodeId].insert(edge.targetNodeId);
		});

	return fileIdToIncludingFileIdMap;
}

std::unordered_map<Id, std::set<Id>> PersistentStorage::getFileIdToImportingFileIdMap() const
{
	std::unordered_map<Id, std::set<Id>> fileIdToImportingFileIdMap;
	{
		std::vector<Id> importedElementIds;
		std::map<Id, std::set<Id>> elementIdToImportingFileIds;

		m_sqliteIndexStorage.forEachOfType<StorageEdge>(
			Edge::typeToInt(Edge::EDGE_IMPORT),
			[&importedElementIds, &elementIdToImportingFileIds](StorageEdge&& edge) {
				importedElementIds.push_back(edge.targetNodeId);
				elementIdToImportingFileIds[edge.targetNodeId].insert(edge.sourceNodeId);
			});

		std::unordered_map<Id, Id> importedElementIdToFileNodeId;
		{
			std::vector<Id> importedSourceLocationIds;
			std::unordered_map<Id, Id> importedSourceLocationToElementIds;
			for (const StorageOccurrence& occurrence:
				 m_sqliteIndexStorage.getOccurrencesForElementIds(importedElementIds))
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
				fileIdToImportingFileIdMap[importedFileIt->second].insert(
					it.second.begin(), it.second.end());
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

std::set<FilePath> PersistentStorage::getReferencedByIncludes(const std::set<FilePath>& filePaths) const
{
	const std::set<Id> ids = getReferenced(
		getFileNodeIds(filePaths), getFileIdToIncludingFileIdMap());

	std::set<FilePath> paths;
	for (Id id: ids)
	{	 // TODO: performance optimize: use just one request for all ids!
		paths.insert(getFileNodePath(id));
	}

	return paths;
}

std::set<FilePath> PersistentStorage::getReferencedByImports(const std::set<FilePath>& filePaths) const
{
	const std::set<Id> ids = getReferenced(
		getFileNodeIds(filePaths), getFileIdToImportingFileIdMap());

	std::set<FilePath> paths;
	for (Id id: ids)
	{
		paths.insert(getFileNodePath(id));
	}

	return paths;
}

std::set<FilePath> PersistentStorage::getReferencingByIncludes(const std::set<FilePath>& filePaths) const
{
	const std::set<Id> ids = getReferencing(
		getFileNodeIds(filePaths), getFileIdToIncludingFileIdMap());

	std::set<FilePath> paths;
	for (Id id: ids)
	{
		paths.insert(getFileNodePath(id));
	}

	return paths;
}

std::set<FilePath> PersistentStorage::getReferencingByImports(const std::set<FilePath>& filePaths) const
{
	const std::set<Id> ids = getReferencing(
		getFileNodeIds(filePaths), getFileIdToImportingFileIdMap());

	std::set<FilePath> paths;
	for (Id id: ids)
	{
		paths.insert(getFileNodePath(id));
	}

	return paths;
}

void PersistentStorage::addNodesToGraph(
	const std::vector<Id>& newNodeIds, Graph* graph, bool addChildCount) const
{
	TRACE();

	std::vector<Id> nodeIds;
	if (graph->getNodeCount())
	{
		for (Id id: newNodeIds)
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

	for (const StorageNode& storageNode: m_sqliteIndexStorage.getAllByIds<StorageNode>(nodeIds))
	{
		const NodeType type(intToNodeKind(storageNode.type));
		if (type.isFile())
		{
			addFileNodeToGraph(storageNode, graph);
		}
		else
		{
			addNodeToGraph(storageNode, type, graph, addChildCount);
		}
	}
}

void PersistentStorage::addFileNodeToGraph(const StorageNode& storageNode, Graph* const graph) const
{
	NameHierarchy nameHierarchy = NameHierarchy::deserialize(storageNode.serializedName);
	const FilePath filePath(nameHierarchy.getRawName());

	bool complete = getFileNodeComplete(storageNode.id);
	bool indexed = getFileNodeIndexed(storageNode.id);

	Node* node = graph->createNode(
		storageNode.id,
		NodeType(NODE_FILE),
		NameHierarchy(filePath.fileName(), NAME_DELIMITER_FILE),
		indexed ? DEFINITION_EXPLICIT : DEFINITION_NONE);
	node->addComponent(std::make_shared<TokenComponentFilePath>(filePath, complete));
}

void PersistentStorage::addNodeToGraph(
	const StorageNode& newNode, const NodeType& type, Graph* graph, bool addChildCount) const
{
	NameHierarchy nameHierarchy = NameHierarchy::deserialize(newNode.serializedName);
	DefinitionKind defKind = DEFINITION_NONE;
	auto it = m_symbolDefinitionKinds.find(newNode.id);
	if (it != m_symbolDefinitionKinds.end())
	{
		defKind = it->second;
	}

	Node* node = graph->createNode(newNode.id, type, std::move(nameHierarchy), defKind);

	if (addChildCount)
	{
		node->setChildCount(m_hierarchyCache.getFirstChildIdsCountForNodeId(newNode.id));
	}
}

void PersistentStorage::addEdgesToGraph(const std::vector<Id>& newEdgeIds, Graph* graph) const
{
	TRACE();

	std::vector<Id> edgeIds;
	for (Id id: newEdgeIds)
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

	for (const StorageEdge& storageEdge: m_sqliteIndexStorage.getAllByIds<StorageEdge>(edgeIds))
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
	const std::vector<Id>& nodeIds, const std::vector<Id>& edgeIds, Graph* graph, bool addChildCount) const
{
	TRACE();

	std::set<Id> allNodeIds(nodeIds.begin(), nodeIds.end());
	std::set<Id> allEdgeIds(edgeIds.begin(), edgeIds.end());

	if (edgeIds.size() > 0)
	{
		for (const StorageEdge& storageEdge: m_sqliteIndexStorage.getAllByIds<StorageEdge>(edgeIds))
		{
			allNodeIds.insert(storageEdge.sourceNodeId);
			allNodeIds.insert(storageEdge.targetNodeId);
		}
	}

	std::set<Id> parentNodeIds;
	for (Id nodeId: allNodeIds)
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
	for (const StorageEdge& edge: edgesToAggregate)
	{
		bool isSource = nodeId == edge.sourceNodeId;
		EdgeInfo edgeInfo;
		edgeInfo.edgeId = edge.id;
		edgeInfo.forward = isSource;
		connectedNodeIds[isSource ? edge.targetNodeId : edge.sourceNodeId].push_back(edgeInfo);
	}

	const std::vector<StorageEdge> outgoingEdges = m_sqliteIndexStorage.getEdgesBySourceIds(
		childNodeIds);
	for (const StorageEdge& outEdge: outgoingEdges)
	{
		EdgeInfo edgeInfo;
		edgeInfo.edgeId = outEdge.id;
		edgeInfo.forward = true;
		connectedNodeIds[outEdge.targetNodeId].push_back(edgeInfo);
	}

	const std::vector<StorageEdge> incomingEdges = m_sqliteIndexStorage.getEdgesByTargetIds(
		childNodeIds);
	for (const StorageEdge& inEdge: incomingEdges)
	{
		EdgeInfo edgeInfo;
		edgeInfo.edgeId = inEdge.id;
		edgeInfo.forward = false;
		connectedNodeIds[inEdge.sourceNodeId].push_back(edgeInfo);
	}

	// get all parent nodes of all connected nodes (up to last level except namespace/undefined)
	const Id nodeParentNodeId = m_hierarchyCache.getLastVisibleParentNodeId(nodeId);

	std::map<Id, std::vector<EdgeInfo>> connectedParentNodeIds;
	for (const std::pair<Id, std::vector<EdgeInfo>>& p: connectedNodeIds)
	{
		const Id parentNodeId = m_hierarchyCache.getLastVisibleParentNodeId(p.first);

		if (parentNodeId != nodeParentNodeId)
		{
			utility::append(connectedParentNodeIds[parentNodeId], p.second);
		}
	}

	// add hierarchies of these parents
	std::vector<Id> nodeIdsToAdd;
	for (const std::pair<Id, std::vector<EdgeInfo>>& p: connectedParentNodeIds)
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
	for (const std::pair<Id, std::vector<EdgeInfo>>& p: connectedParentNodeIds)
	{
		const Id aggregationTargetNodeId = p.first;

		Node* targetNode = graph->getNodeById(aggregationTargetNodeId);
		if (!targetNode)
		{
			LOG_ERROR("Aggregation target node not present.");
		}

		std::shared_ptr<TokenComponentAggregation> componentAggregation =
			std::make_shared<TokenComponentAggregation>();
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

	std::shared_ptr<SourceLocationFile> locationFile =
		m_sqliteIndexStorage.getSourceLocationsForFile(path);
	locationFile->forEachStartSourceLocation([this, &tokenIds, &tokenIdsSet](SourceLocation* location) {
		if (location->getType() != LOCATION_TOKEN)
		{
			return;
		}

		for (Id tokenId: location->getTokenIds())
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
	});

	addNodesWithParentsAndEdgesToGraph(tokenIds, {}, graph, true);

	Node* fileNode = graph->getNodeById(fileId);
	Id memberEdgeId = 0;
	for (Id tokenId: tokenIds)
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
	graph->forEachNode([&nodeIds](Node* node) { nodeIds.push_back(node->getId()); });

	std::vector<StorageComponentAccess> accesses =
		m_sqliteIndexStorage.getComponentAccessesByNodeIds(nodeIds);
	for (const StorageComponentAccess& access: accesses)
	{
		if (access.nodeId != 0)
		{
			graph->getNodeById(access.nodeId)
				->addComponent(std::make_shared<TokenComponentAccess>(intToAccessKind(access.type)));
		}
	}
}

void PersistentStorage::addComponentIsAmbiguousToGraph(Graph* graph) const
{
	TRACE();

	std::vector<Id> edgeIds;
	graph->forEachEdge([&edgeIds](Edge* edge) { edgeIds.push_back(edge->getId()); });

	int componentKind = elementComponentKindToInt(ElementComponentKind::IS_AMBIGUOUS);
	for (const StorageElementComponent& component:
		 m_sqliteIndexStorage.getElementComponentsByElementIds(edgeIds))
	{
		if (component.type == componentKind)
		{
			graph->getEdgeById(component.elementId)
				->addComponent(std::make_shared<TokenComponentIsAmbiguous>());
		}
	}
}

void PersistentStorage::addCompleteFlagsToSourceLocationCollection(
	SourceLocationCollection* collection) const
{
	TRACE();

	collection->forEachSourceLocationFile([this](std::shared_ptr<SourceLocationFile> file) {
		Id fileId = getFileNodeId(file->getFilePath());
		file->setIsComplete(getFileNodeComplete(fileId));
		file->setIsIndexed(getFileNodeIndexed(fileId));
		file->setLanguage(getFileNodeLanguage(fileId));
	});
}

void PersistentStorage::addInheritanceChainsToGraph(const std::vector<Id>& activeNodeIds, Graph* graph) const
{
	TRACE();

	std::set<Id> activeNodeIdsSet;
	for (Id activeNodeId: activeNodeIds)
	{
		std::set<Id> visibleParentIds, edgeIds;
		visibleParentIds.insert(activeNodeId);
		m_hierarchyCache.addAllVisibleParentIdsForNodeId(activeNodeId, &visibleParentIds, &edgeIds);

		for (Id nodeId: visibleParentIds)
		{
			Node* node = graph->getNodeById(nodeId);
			if (node && node->getType().isInheritable())
			{
				activeNodeIdsSet.insert(node->getId());
			}
		}
	}

	std::set<Id> nodeIdsSet;
	graph->forEachNode([&nodeIdsSet, &activeNodeIdsSet](Node* node) {
		if (node->getType().isInheritable() &&
			activeNodeIdsSet.find(node->getId()) == activeNodeIdsSet.end())
		{
			nodeIdsSet.insert(node->getId());
		}
	});

	std::vector<std::set<Id>*> nodeIdSets;
	nodeIdSets.push_back(&activeNodeIdsSet);
	nodeIdSets.push_back(&nodeIdsSet);

	size_t inheritanceEdgeCount = 1;

	for (size_t i = 0; i < nodeIdSets.size(); i++)
	{
		for (const Id nodeId: *nodeIdSets[i])
		{
			for (const std::tuple<Id, Id, std::vector<Id>>& edge:
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
					inheritanceEdgeId,
					Edge::EDGE_INHERITANCE,
					graph->getNodeById(sourceId),
					graph->getNodeById(targetId));

				inheritanceEdge->addComponent(
					std::make_shared<TokenComponentInheritanceChain>(edgeIds));
			}
		}
	}
}

void PersistentStorage::buildFilePathMaps()
{
	TRACE();

	m_sqliteIndexStorage.forEach<StorageFile>([&](StorageFile&& file) {
		const FilePath path(file.filePath);

		m_fileNodeIds.emplace(path, file.id);
		m_lowerCasefileNodeIds.emplace(path.getLowerCase(), file.id);
		m_fileNodePaths.emplace(file.id, path);
		m_fileNodeComplete.emplace(file.id, file.complete);
		m_fileNodeIndexed.emplace(file.id, file.indexed);
		m_fileNodeLanguage.emplace(file.id, file.languageIdentifier);

		if (!m_hasJavaFiles && path.extension() == L".java")
		{
			m_hasJavaFiles = true;
		}
	});

	m_sqliteIndexStorage.forEach<StorageSymbol>([&](StorageSymbol&& symbol) {
		m_symbolDefinitionKinds.emplace(symbol.id, intToDefinitionKind(symbol.definitionKind));
	});
}

void PersistentStorage::buildSearchIndex()
{
	TRACE();

	const FilePath dbPath = getIndexDbFilePath();

	m_sqliteIndexStorage.forEach<StorageNode>([&](StorageNode&& node) {
		const NodeType type(intToNodeKind(node.type));
		if (type.isFile())
		{
			bool indexed = getFileNodeIndexed(node.id);
			if (!indexed)
			{
				return;
			}

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
			const DefinitionKind defKind =
				(it != m_symbolDefinitionKinds.end() ? it->second : DEFINITION_NONE);
			if (defKind != DEFINITION_IMPLICIT)
			{
				const NameHierarchy nameHierarchy = NameHierarchy::deserialize(node.serializedName);

				// we don't use the signature here, so elements with the same signature share the
				// same node.
				std::wstring name = nameHierarchy.getQualifiedName();

				// replace template arguments with .. to avoid clutter in search results and have
				// different template specializations share the same node.
				if (defKind == DEFINITION_NONE &&
					nameHierarchy.getDelimiter() == nameDelimiterTypeToString(NAME_DELIMITER_CXX))
				{
					name = utility::replaceBetween(name, L'<', L'>', L"..");
				}

				m_symbolIndex.addNode(node.id, std::move(name), type);
			}
		}
	});

	m_symbolIndex.finishSetup();
	m_fileIndex.finishSetup();
}

void PersistentStorage::buildFullTextSearchIndex() const
{
	TRACE();

	TextCodec codec(ApplicationSettings::getInstance()->getTextEncoding());

	m_fullTextSearchCodec = codec.getName();

	m_fullTextSearchIndex.clear();

	std::vector<std::shared_ptr<std::thread>> threads;
	{
		std::vector<StorageFile> indexedFiles;
		for (const StorageFile& file: m_sqliteIndexStorage.getAll<StorageFile>())
		{
			if (file.indexed)
			{
				indexedFiles.push_back(file);
			}
		}
		for (std::vector<StorageFile> part:
			 utility::splitToEqualySizedParts(indexedFiles, utility::getIdealThreadCount()))
		{
			std::shared_ptr<std::thread> thread = std::make_shared<std::thread>(
				[&](const std::vector<StorageFile>& files) {
					for (const StorageFile& file: files)
					{
						m_fullTextSearchIndex.addFile(
							file.id,
							codec.decode(m_sqliteIndexStorage.getFileContentById(file.id)->getText()));
					}
				},
				part);
			threads.push_back(thread);
		}
	}
	for (std::shared_ptr<std::thread> thread: threads)
	{
		thread->join();
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

	m_sqliteIndexStorage.forEachOfType<StorageEdge>(
		Edge::typeToInt(Edge::EDGE_MEMBER),
		[&childNodeIds, &childIdToMemberEdgeIdMap](StorageEdge&& edge) {
			childNodeIds.push_back(edge.targetNodeId);
			childIdToMemberEdgeIdMap.emplace(edge.targetNodeId, edge.id);
		});

	std::vector<Id> locationIds;
	std::unordered_map<Id, Id> locationIdToElementIdMap;
	for (const StorageOccurrence& occurrence:
		 m_sqliteIndexStorage.getOccurrencesForElementIds(childNodeIds))
	{
		locationIds.push_back(occurrence.sourceLocationId);
		locationIdToElementIdMap.emplace(occurrence.sourceLocationId, occurrence.elementId);
	}

	SourceLocationCollection collection;
	for (const StorageSourceLocation& location:
		 m_sqliteIndexStorage.getAllByIds<StorageSourceLocation>(locationIds))
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
				location.endCol);
		}
	}

	// Set first 3 bits to 1 to avoid collisions
	Id baseId = ~(~Id(0) >> 3) + 1;

	collection.forEachSourceLocation([&](SourceLocation* location) {
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
	});
}

void PersistentStorage::buildHierarchyCache()
{
	TRACE();

	std::vector<Id> sourceNodeIds;
	std::vector<StorageEdge> memberEdges;

	m_sqliteIndexStorage.forEachOfType<StorageEdge>(
		Edge::typeToInt(Edge::EDGE_MEMBER), [&sourceNodeIds, &memberEdges](StorageEdge&& edge) {
			sourceNodeIds.push_back(edge.sourceNodeId);
			memberEdges.emplace_back(edge);
		});

	std::set<Id> invisibleParentSourceNodeIds;

	m_sqliteIndexStorage.forEachByIds<StorageNode>(
		sourceNodeIds, [&invisibleParentSourceNodeIds](StorageNode&& node) {
			if (!NodeType(intToNodeKind(node.type)).isVisibleAsParentInGraph())
			{
				invisibleParentSourceNodeIds.insert(node.id);
			}
		});

	for (const StorageEdge& edge: memberEdges)
	{
		bool sourceIsVisible = true;
		if (invisibleParentSourceNodeIds.find(edge.sourceNodeId) != invisibleParentSourceNodeIds.end())
		{
			sourceIsVisible = false;
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
			edge.id,
			edge.sourceNodeId,
			edge.targetNodeId,
			sourceIsVisible,
			sourceIsImplicit,
			targetIsImplicit);
	}

	m_sqliteIndexStorage.forEachOfType<StorageEdge>(
		Edge::typeToInt(Edge::EDGE_INHERITANCE), [this](StorageEdge&& edge) {
			m_hierarchyCache.createInheritance(edge.id, edge.sourceNodeId, edge.targetNodeId);
		});
}
