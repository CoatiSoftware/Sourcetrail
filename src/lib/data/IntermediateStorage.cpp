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

Id IntermediateStorage::addFile(const std::string& name, const std::string& filePath, const std::string& modificationTime)
{
	std::shared_ptr<StorageFile> file = std::make_shared<StorageFile>(0, name, filePath, modificationTime);

	std::string serialized = serialize(*(file.get()));
	std::unordered_map<std::string, Id>::const_iterator it = m_fileNamesToIds.find(serialized);
	if (it != m_fileNamesToIds.end())
	{
		Id id = it->second;
		if (m_fileIdsToData[id]->filePath.size() == 0) // stored information is incomplete.
		{
			m_fileIdsToData[id]->filePath = filePath; // so we replace it.
		}
		if (m_fileIdsToData[id]->modificationTime.size() == 0) // stored information is incomplete.
		{
			m_fileIdsToData[id]->modificationTime = modificationTime; // so we replace it.
		}
		return id;
	}

	Id id = m_nextId++;
	m_fileNamesToIds[serialized] = id;
	m_fileIdsToData[id] = file;

	return id;
}

Id IntermediateStorage::addNode(int type, const std::string& serializedName, int definitionType)
{
	std::shared_ptr<StorageNode> node = std::make_shared<StorageNode>(0, type, serializedName, definitionType);

	std::string serialized = serialize(*(node.get()));
	std::unordered_map<std::string, Id>::const_iterator it = m_nodeNamesToIds.find(serialized);
	if (it != m_nodeNamesToIds.end())
	{
		std::map<Id, std::shared_ptr<StorageNode>>::const_iterator it2 = m_nodeIdsToData.find(it->second);
		std::shared_ptr<StorageNode> storageNode = it2->second;
		if (storageNode->definitionType == 0)
		{
			if (definitionType > 0)
			{
				storageNode->definitionType = definitionType;
			}

			if (storageNode->type < type)
			{
				storageNode->type = type;
			}
		}
		return it->second;
	}

	Id id = m_nextId++;
	m_nodeNamesToIds[serialized] = id;
	m_nodeIdsToData[id] = node;

	return id;
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

	Id id = m_nextId++;
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

	Id id = m_nextId++;
	m_localSymbolNamesToIds[serialized] = id;
	m_localSymbolIdsToData[id] = localSymbol;

	return id;
}

void IntermediateStorage::addSourceLocation(Id elementId, Id fileNodeId, uint startLine, uint startCol, uint endLine, uint endCol, int type)
{
	m_sourceLocations.push_back(StorageSourceLocation(
		0,
		elementId,
		fileNodeId,
		startLine,
		startCol,
		endLine,
		endCol,
		type
	));
}

void IntermediateStorage::addComponentAccess(Id edgeId, int type)
{
	m_componentAccesses.push_back(StorageComponentAccess(edgeId, type));
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

void IntermediateStorage::addError(const std::string& message, bool fatal, bool indexed, const std::string& filePath, uint startLine, uint startCol)
{
	m_errors.push_back(StorageError(
		message,
		fatal,
		indexed,
		filePath,
		startLine,
		startCol
	));
}

void IntermediateStorage::forEachFile(std::function<void(const Id /*id*/, const StorageFile& /*data*/)> callback) const
{
	for (std::unordered_map<Id, std::shared_ptr<StorageFile>>::const_iterator it = m_fileIdsToData.begin(); it != m_fileIdsToData.end(); it++)
	{
		callback(it->first, *(it->second.get()));
	}
}

void IntermediateStorage::forEachNode(std::function<void(const Id /*id*/, const StorageNode& /*data*/)> callback) const
{
	for (std::map<Id, std::shared_ptr<StorageNode>>::const_iterator it = m_nodeIdsToData.begin(); it != m_nodeIdsToData.end(); it++)
	{
		callback(it->first, *(it->second.get()));
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

void IntermediateStorage::forEachSourceLocation(std::function<void(const StorageSourceLocation& /*data*/)> callback) const
{
	for (std::vector<StorageSourceLocation>::const_iterator it = m_sourceLocations.begin(); it != m_sourceLocations.end(); it++)
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

std::string IntermediateStorage::serialize(const StorageEdge& edge)
{
	return (
		std::to_string(edge.type) +
		std::to_string(edge.sourceNodeId) +
		std::to_string(edge.targetNodeId)
	);
}

std::string IntermediateStorage::serialize(const StorageNode& node)
{
	return node.serializedName;
}

std::string IntermediateStorage::serialize(const StorageFile& file)
{
	return file.filePath;
}

std::string IntermediateStorage::serialize(const StorageLocalSymbol& localSymbol)
{
	return localSymbol.name;
}
