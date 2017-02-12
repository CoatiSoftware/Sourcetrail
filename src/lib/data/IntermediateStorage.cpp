#include "data/IntermediateStorage.h"

#include "data/graph/Edge.h"
#include "utility/logging/logging.h"

IntermediateStorage::IntermediateStorage()
	: m_nextId(1)
{
}

IntermediateStorage::~IntermediateStorage()
{
}

void IntermediateStorage::clear()
{
	m_nodeNamesToIds.clear();
	m_nodeIdsToData.clear();
	m_files.clear();
	m_symbols.clear();
	m_edgeNamesToIds.clear();
	m_edgeIdsToData.clear();
	m_localSymbolNamesToIds.clear();
	m_localSymbolIdsToData.clear();
	m_sourceLocationNamesToIds.clear();
	m_sourceLocationIdsToData.clear();
	m_occurrences.clear();
	m_componentAccesses.clear();
	m_commentLocations.clear();
	m_errors.clear();
	m_nextId = 1;
}

size_t IntermediateStorage::getSourceLocationCount() const
{
	return m_sourceLocationNamesToIds.size();
}

Id IntermediateStorage::addNode(int type, const std::string& serializedName)
{
	std::shared_ptr<StorageNode> node = std::make_shared<StorageNode>(0, type, serializedName);

	std::string serialized = serialize(*(node.get()));
	std::unordered_map<std::string, Id>::const_iterator it = m_nodeNamesToIds.find(serialized);
	if (it != m_nodeNamesToIds.end())
	{
		std::map<Id, std::shared_ptr<StorageNode>>::const_iterator it2 = m_nodeIdsToData.find(it->second);
		std::shared_ptr<StorageNode> storedNode = it2->second;
		if (storedNode->type < type)
		{
			storedNode->type = type;
		}
		return it->second;
	}

	const Id id = m_nextId++;
	m_nodeNamesToIds[serialized] = id;
	m_nodeIdsToData[id] = node;

	return id;
}

void IntermediateStorage::addFile(const Id id, const std::string& filePath, const std::string& modificationTime)
{
	m_files.push_back(StorageFile(id, filePath, modificationTime));
}

void IntermediateStorage::addSymbol(const Id id, int definitionKind)
{
	m_symbols.push_back(StorageSymbol(id, definitionKind));
}

Id IntermediateStorage::addEdge(int type, Id sourceId, Id targetId)
{
	std::shared_ptr<StorageEdge> edge = std::make_shared<StorageEdge>(0, type, sourceId, targetId);

	std::string serialized = serialize(*(edge.get()));
	std::unordered_map<std::string, Id>::const_iterator it = m_edgeNamesToIds.find(serialized);
	if (it != m_edgeNamesToIds.end())
	{
		return it->second;
	}

	const Id id = m_nextId++;
	m_edgeNamesToIds[serialized] = id;
	m_edgeIdsToData[id] = edge;

	return id;
}

Id IntermediateStorage::addLocalSymbol(const std::string& name)
{
	std::shared_ptr<StorageLocalSymbol> localSymbol = std::make_shared<StorageLocalSymbol>(0, name);

	std::string serialized = serialize(*(localSymbol.get()));
	std::unordered_map<std::string, Id>::const_iterator it = m_localSymbolNamesToIds.find(serialized);
	if (it != m_localSymbolNamesToIds.end())
	{
		return it->second;
	}

	const Id id = m_nextId++;
	m_localSymbolNamesToIds[serialized] = id;
	m_localSymbolIdsToData[id] = localSymbol;

	return id;
}

Id IntermediateStorage::addSourceLocation(Id fileNodeId, uint startLine, uint startCol, uint endLine, uint endCol, int type)
{
	std::shared_ptr<StorageSourceLocation> sourceLocation = std::make_shared<StorageSourceLocation>(
		0,
		fileNodeId,
		startLine,
		startCol,
		endLine,
		endCol,
		type
	);

	std::string serialized = serialize(*(sourceLocation.get()));
	std::unordered_map<std::string, Id>::const_iterator it = m_sourceLocationNamesToIds.find(serialized);
	if (it != m_sourceLocationNamesToIds.end())
	{
		return it->second;
	}

	const Id id = m_nextId++;
	m_sourceLocationNamesToIds[serialized] = id;
	m_sourceLocationIdsToData[id] = sourceLocation;

	return id;
}

void IntermediateStorage::addOccurrence(Id elementId, Id sourceLocationId)
{
	m_occurrences.push_back(StorageOccurrence(elementId, sourceLocationId));
}

void IntermediateStorage::addComponentAccess(Id nodeId, int type)
{
	m_componentAccesses.push_back(StorageComponentAccess(nodeId, type));
}

void IntermediateStorage::addCommentLocation(Id fileNodeId, uint startLine, uint startCol, uint endLine, uint endCol)
{
	m_commentLocations.push_back(StorageCommentLocation(
		0,
		fileNodeId,
		startLine,
		startCol,
		endLine,
		endCol
	));
}

void IntermediateStorage::addError(const std::string& message, const FilePath& filePath, uint startLine, uint startCol, bool fatal, bool indexed)
{
	m_errors.push_back(StorageError(
		0,
		message,
		filePath,
		startLine,
		startCol,
		fatal,
		indexed
	));
}

Id IntermediateStorage::addBookmark(const Bookmark& bookmark)
{
	m_bookmarks.push_back(bookmark);

	return -1;
}

void IntermediateStorage::forEachNode(std::function<void(const Id /*id*/, const StorageNode& /*data*/)> callback) const
{
	for (std::map<Id, std::shared_ptr<StorageNode>>::const_iterator it = m_nodeIdsToData.begin(); it != m_nodeIdsToData.end(); it++)
	{
		callback(it->first, *(it->second.get()));
	}
}

void IntermediateStorage::forEachFile(std::function<void(const StorageFile& /*data*/)> callback) const
{
	for (std::vector<StorageFile>::const_iterator it = m_files.begin(); it != m_files.end(); it++)
	{
		callback(*it);
	}
}

void IntermediateStorage::forEachSymbol(std::function<void(const StorageSymbol& /*data*/)> callback) const
{
	for (std::vector<StorageSymbol>::const_iterator it = m_symbols.begin(); it != m_symbols.end(); it++)
	{
		callback(*it);
	}
}

void IntermediateStorage::forEachEdge(std::function<void(const Id /*id*/, const StorageEdge& /*data*/)> callback) const
{
	for (std::map<Id, std::shared_ptr<StorageEdge>>::const_iterator it = m_edgeIdsToData.begin(); it != m_edgeIdsToData.end(); it++)
	{
		callback(it->first, *(it->second.get()));
	}
}

void IntermediateStorage::forEachLocalSymbol(std::function<void(const Id /*id*/, const StorageLocalSymbol& /*data*/)> callback) const
{
	for (std::map<Id, std::shared_ptr<StorageLocalSymbol>>::const_iterator it = m_localSymbolIdsToData.begin(); it != m_localSymbolIdsToData.end(); it++)
	{
		callback(it->first, *(it->second.get()));
	}
}

void IntermediateStorage::forEachSourceLocation(std::function<void(const Id /*id*/, const StorageSourceLocation& /*data*/)> callback) const
{
	for (std::map<Id, std::shared_ptr<StorageSourceLocation>>::const_iterator it = m_sourceLocationIdsToData.begin(); it != m_sourceLocationIdsToData.end(); it++)
	{
		callback(it->first, *(it->second.get()));
	}
}

void IntermediateStorage::forEachOccurrence(std::function<void(const StorageOccurrence& /*data*/)> callback) const
{
	for (std::vector<StorageOccurrence>::const_iterator it = m_occurrences.begin(); it != m_occurrences.end(); it++)
	{
		callback(*it);
	}
}

void IntermediateStorage::forEachComponentAccess(std::function<void(const StorageComponentAccess& /*data*/)> callback) const
{
	for (std::vector<StorageComponentAccess>::const_iterator it = m_componentAccesses.begin(); it != m_componentAccesses.end(); it++)
	{
		callback(*it);
	}
}

void IntermediateStorage::forEachCommentLocation(std::function<void(const StorageCommentLocation& /*data*/)> callback) const
{
	for (std::vector<StorageCommentLocation>::const_iterator it = m_commentLocations.begin(); it != m_commentLocations.end(); it++)
	{
		callback(*it);
	}
}

void IntermediateStorage::forEachError(std::function<void(const StorageError& /*data*/)> callback) const
{
	for (std::vector<StorageError>::const_iterator it = m_errors.begin(); it != m_errors.end(); it++)
	{
		callback(*it);
	}
}

std::string IntermediateStorage::serialize(const StorageEdge& edge) const
{
	return (
		std::to_string(edge.type) +
		std::to_string(edge.sourceNodeId) +
		std::to_string(edge.targetNodeId)
	);
}

std::string IntermediateStorage::serialize(const StorageNode& node) const
{
	return node.serializedName;
}

std::string IntermediateStorage::serialize(const StorageLocalSymbol& localSymbol) const
{
	return localSymbol.name;
}

std::string IntermediateStorage::serialize(const StorageSourceLocation& sourceLocation) const
{
	return (
		std::to_string(sourceLocation.fileNodeId) +
		std::to_string(sourceLocation.startLine) +
		std::to_string(sourceLocation.startCol) +
		std::to_string(sourceLocation.endLine) +
		std::to_string(sourceLocation.endCol) +
		std::to_string(sourceLocation.type)
	);
}
