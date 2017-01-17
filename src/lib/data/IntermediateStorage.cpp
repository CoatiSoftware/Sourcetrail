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
	m_fileNamesToIds.clear();
	m_fileIdsToData.clear();
	m_symbolNamesToIds.clear();
	m_symbolIdsToData.clear();
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

Id IntermediateStorage::addFile(const std::string& serializedName, const std::string& filePath, const std::string& modificationTime)
{
	std::shared_ptr<StorageFile> file = std::make_shared<StorageFile>(0, serializedName, filePath, modificationTime);

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

Id IntermediateStorage::addSymbol(int type, const std::string& serializedName, int definitionType)
{
	std::shared_ptr<StorageSymbol> symbol = std::make_shared<StorageSymbol>(0, type, serializedName, definitionType);

	std::string serialized = serialize(*(symbol.get()));
	std::unordered_map<std::string, Id>::const_iterator it = m_symbolNamesToIds.find(serialized);
	if (it != m_symbolNamesToIds.end())
	{
		std::map<Id, std::shared_ptr<StorageSymbol>>::const_iterator it2 = m_symbolIdsToData.find(it->second);
		std::shared_ptr<StorageSymbol> storedSymbol = it2->second;
		if (storedSymbol->definitionType == 0)
		{
			if (definitionType > 0)
			{
				storedSymbol->definitionType = definitionType;
			}

			if (storedSymbol->type < type)
			{
				storedSymbol->type = type;
			}
		}
		return it->second;
	}

	Id id = m_nextId++;
	m_symbolNamesToIds[serialized] = id;
	m_symbolIdsToData[id] = symbol;

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

	Id id = m_nextId++;
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

void IntermediateStorage::forEachFile(std::function<void(const Id /*id*/, const StorageFile& /*data*/)> callback) const
{
	for (std::unordered_map<Id, std::shared_ptr<StorageFile>>::const_iterator it = m_fileIdsToData.begin(); it != m_fileIdsToData.end(); it++)
	{
		callback(it->first, *(it->second.get()));
	}
}

void IntermediateStorage::forEachSymbol(std::function<void(const Id /*id*/, const StorageSymbol& /*data*/)> callback) const
{
	for (std::map<Id, std::shared_ptr<StorageSymbol>>::const_iterator it = m_symbolIdsToData.begin(); it != m_symbolIdsToData.end(); it++)
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

std::string IntermediateStorage::serialize(const StorageSymbol& symbol) const
{
	return symbol.serializedName;
}

std::string IntermediateStorage::serialize(const StorageFile& file) const
{
	return file.filePath;
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
