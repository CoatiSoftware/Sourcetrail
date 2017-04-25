#include "data/IntermediateStorage.h"

#include <set>

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

void IntermediateStorage::setAllFilesIncomplete()
{
	for (StorageFile& file : m_files)
	{
		file.complete = false;
	}
}

void IntermediateStorage::setFilesWithErrorsIncomplete()
{
	std::set<FilePath> errorFiles;
	for (StorageError& error : m_errors)
	{
		errorFiles.insert(error.filePath);
	}

	for (StorageFile& file : m_files)
	{
		if (errorFiles.find(file.filePath) != errorFiles.end())
		{
			file.complete = false;
		}
	}
}

Id IntermediateStorage::addNode(int type, const std::string& serializedName)
{
	std::shared_ptr<StorageNode> node = std::make_shared<StorageNode>(0, type, serializedName);

	const std::string serialized = serialize(*(node.get()));
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

void IntermediateStorage::addFile(const Id id, const std::string& filePath, const std::string& modificationTime, bool complete)
{
	const StorageFile file(id, filePath, modificationTime, complete);
	const std::string serialized = serialize(file);

	if (m_serializedFiles.find(serialized) == m_serializedFiles.end())
	{
		m_files.push_back(file);
		m_serializedFiles.insert(serialized);

	}
}

void IntermediateStorage::addSymbol(const Id id, int definitionKind)
{
	m_symbols.push_back(StorageSymbol(id, definitionKind));
}

Id IntermediateStorage::addEdge(int type, Id sourceId, Id targetId)
{
	std::shared_ptr<StorageEdge> edge = std::make_shared<StorageEdge>(0, type, sourceId, targetId);

	const std::string serialized = serialize(*(edge.get()));
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

	const std::string serialized = serialize(*(localSymbol.get()));
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

	const std::string serialized = serialize(*(sourceLocation.get()));
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
	const StorageOccurrence occurrence(elementId, sourceLocationId);
	const std::string serialized = serialize(occurrence);

	if (m_serializedOccurrences.find(serialized) == m_serializedOccurrences.end())
	{
		m_occurrences.push_back(occurrence);
		m_serializedOccurrences.insert(serialized);
	}
}

void IntermediateStorage::addComponentAccess(Id nodeId, int type)
{
	const StorageComponentAccess componentAccess(0, nodeId, type);
	const std::string serialized = serialize(componentAccess);

	if (m_serializedComponentAccesses.find(serialized) == m_serializedComponentAccesses.end())
	{
		m_componentAccesses.push_back(componentAccess);
		m_serializedComponentAccesses.insert(serialized);
	}
}

void IntermediateStorage::addCommentLocation(Id fileNodeId, uint startLine, uint startCol, uint endLine, uint endCol)
{
	const StorageCommentLocation commentLocation(
		0,
		fileNodeId,
		startLine,
		startCol,
		endLine,
		endCol
	);
	const std::string serialized = serialize(commentLocation);

	if (m_serializedCommentLocations.find(serialized) == m_serializedCommentLocations.end())
	{
		m_commentLocations.push_back(commentLocation);
		m_serializedCommentLocations.insert(serialized);

	}
}

void IntermediateStorage::addError(const std::string& message, const FilePath& filePath, uint startLine, uint startCol, bool fatal, bool indexed)
{
	const StorageError error(
		0,
		message,
		filePath,
		startLine,
		startCol,
		fatal,
		indexed
	);
	const std::string serialized = serialize(error);

	if (m_serializedErrors.find(serialized) == m_serializedErrors.end())
	{
		m_errors.push_back(error);
		m_serializedErrors.insert(serialized);
	}
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

std::string IntermediateStorage::serialize(const StorageNode& node) const
{
	return node.serializedName;
}

std::string IntermediateStorage::serialize(const StorageFile& file) const
{
	return file.filePath;
}

std::string IntermediateStorage::serialize(const StorageEdge& edge) const
{
	return (
		std::to_string(edge.type) + ";" +
		std::to_string(edge.sourceNodeId) + ";" +
		std::to_string(edge.targetNodeId)
		);
}

std::string IntermediateStorage::serialize(const StorageLocalSymbol& localSymbol) const
{
	return localSymbol.name;
}

std::string IntermediateStorage::serialize(const StorageSourceLocation& sourceLocation) const
{
	return (
		std::to_string(sourceLocation.fileNodeId) + ";" +
		std::to_string(sourceLocation.startLine) + ";" +
		std::to_string(sourceLocation.startCol) + ";" +
		std::to_string(sourceLocation.endLine) + ";" +
		std::to_string(sourceLocation.endCol) + ";" +
		std::to_string(sourceLocation.type)
	);
}

std::string IntermediateStorage::serialize(const StorageOccurrence& occurrence) const
{
	return std::to_string(occurrence.elementId) +  ";" + std::to_string(occurrence.sourceLocationId);
}


std::string IntermediateStorage::serialize(const StorageComponentAccess& componentAccess) const
{
	return std::to_string(componentAccess.nodeId);
}


std::string IntermediateStorage::serialize(const StorageCommentLocation& commentLocation) const
{
	return (
		std::to_string(commentLocation.fileNodeId) + ";" +
		std::to_string(commentLocation.startLine) + ";" +
		std::to_string(commentLocation.startCol) + ";" +
		std::to_string(commentLocation.endLine) + ";" +
		std::to_string(commentLocation.endCol)
	);
}


std::string IntermediateStorage::serialize(const StorageError& error) const
{
	return (
		error.message + ";" +
		std::to_string(error.fatal) + ";" +
		error.filePath.str() + ";" +
		std::to_string(error.lineNumber) + ";" +
		std::to_string(error.columnNumber)
	);
}



