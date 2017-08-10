#include "data/storage/IntermediateStorage.h"

#include <set>

IntermediateStorage::IntermediateStorage()
	: m_nextId(1)
{
}

IntermediateStorage::~IntermediateStorage()
{
}

void IntermediateStorage::clear()
{
	m_nodes.clear();
	m_nodesIndex.clear();
	m_files.clear();
	m_symbols.clear();
	m_edges.clear();
	m_edgesIndex.clear();
	m_localSymbols.clear();
	m_sourceLocations.clear();
	m_occurrences.clear();
	m_componentAccesses.clear();
	m_commentLocations.clear();
	m_errors.clear();
	m_nextId = 1;
}

size_t IntermediateStorage::getByteSize() const
{
	unsigned int byteSize = 0;

	for (const StorageFile& storageFile: getStorageFiles())
	{
		byteSize += sizeof(StorageFile);
		byteSize += storageFile.filePath.size();
		byteSize += storageFile.modificationTime.size();
	}

	for (const StorageError& storageError: getErrors())
	{
		byteSize += sizeof(StorageError);
		byteSize += storageError.filePath.str().size();
		byteSize += storageError.message.size();
		byteSize += storageError.commandline.size();
	}

	for (const StorageNode& storageNode: getStorageNodes())
	{
		byteSize += sizeof(StorageNode);
		byteSize += storageNode.serializedName.size();
	}

	for (const StorageLocalSymbol& storageLocalSymbol: getStorageLocalSymbols())
	{
		byteSize += sizeof(StorageLocalSymbol);
		byteSize += storageLocalSymbol.name.size();
	}

	byteSize += sizeof(StorageEdge) * getStorageEdges().size();
	byteSize += sizeof(StorageCommentLocation) * getCommentLocations().size();
	byteSize += sizeof(StorageComponentAccess) * getComponentAccesses().size();
	byteSize += sizeof(StorageOccurrence) * getStorageOccurrences().size();
	byteSize += sizeof(StorageSymbol) * getStorageSymbols().size();
	byteSize += sizeof(StorageSourceLocation) * getStorageSourceLocations().size();

	return byteSize;
}

size_t IntermediateStorage::getSourceLocationCount() const
{
	return m_sourceLocations.size();
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
	std::set<std::string> errorFileNames;
	for (const StorageError& error : m_errors)
	{
		errorFileNames.insert(error.filePath.str());
	}

	for (StorageFile& file : m_files)
	{
		if (errorFileNames.find(file.filePath) != errorFileNames.end())
		{
			file.complete = false;
		}
	}
}

Id IntermediateStorage::addNode(int type, const std::string& serializedName)
{
	StorageNode node(0, type, serializedName);

	const std::string serialized = serialize(node);
	std::unordered_map<std::string, size_t>::iterator it = m_nodesIndex.find(serialized);
	if (it != m_nodesIndex.end())
	{
		StorageNode& storedNode = m_nodes[it->second];
		if (storedNode.type < type)
		{
			storedNode.type = type;
		}
		return storedNode.id;
	}

	const Id id = m_nextId++;
	node.id = id;
	m_nodes.push_back(node);
	m_nodesIndex.emplace(serialized, m_nodes.size() - 1);
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
	StorageEdge edge = StorageEdge(0, type, sourceId, targetId);

	const std::string serialized = serialize(edge);
	std::unordered_map<std::string, size_t>::const_iterator it = m_edgesIndex.find(serialized);
	if (it != m_edgesIndex.end())
	{
		return m_edges[it->second].id;
	}

	const Id id = m_nextId++;
	edge.id = id;
	m_edges.push_back(edge);
	m_edgesIndex.emplace(serialized, m_edges.size() - 1);
	return id;
}

Id IntermediateStorage::addLocalSymbol(const std::string& name)
{
	StorageLocalSymbol localSymbol = StorageLocalSymbol(0, name);

	const std::string serialized = serialize(localSymbol);
	std::unordered_map<std::string, StorageLocalSymbol>::const_iterator it = m_localSymbols.find(serialized);
	if (it != m_localSymbols.end())
	{
		return it->second.id;
	}

	const Id id = m_nextId++;
	localSymbol.id = id;
	m_localSymbols[serialized] = localSymbol;
	return id;
}

Id IntermediateStorage::addSourceLocation(Id fileNodeId, uint startLine, uint startCol, uint endLine, uint endCol, int type)
{
	StorageSourceLocation sourceLocation = StorageSourceLocation(
		0,
		fileNodeId,
		startLine,
		startCol,
		endLine,
		endCol,
		type
	);

	const std::string serialized = serialize(sourceLocation);
	std::unordered_map<std::string, StorageSourceLocation>::const_iterator it = m_sourceLocations.find(serialized);
	if (it != m_sourceLocations.end())
	{
		return it->second.id;
	}

	const Id id = m_nextId++;
	sourceLocation.id = id;
	m_sourceLocations[serialized] = sourceLocation;
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

void IntermediateStorage::addError(
	const std::string& message, const std::string& commandline, const FilePath& filePath,
	uint startLine, uint startCol, bool fatal, bool indexed)
{
	const StorageError error(
		0,
		message,
		commandline,
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

void IntermediateStorage::forEachNode(std::function<void(const StorageNode& /*data*/)> callback) const
{
	for (const StorageNode& node : m_nodes)
	{
		callback(node);
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

void IntermediateStorage::forEachEdge(std::function<void(const StorageEdge& /*data*/)> callback) const
{
	for (const StorageEdge& edge : m_edges)
	{
		callback(edge);
	}
}

void IntermediateStorage::forEachLocalSymbol(std::function<void(const StorageLocalSymbol& /*data*/)> callback) const
{
	for (std::unordered_map<std::string, StorageLocalSymbol>::const_iterator it = m_localSymbols.begin();
		it != m_localSymbols.end(); it++)
	{
		callback(it->second);
	}
}

void IntermediateStorage::forEachSourceLocation(std::function<void(const StorageSourceLocation& /*data*/)> callback) const
{
	for (std::unordered_map<std::string, StorageSourceLocation>::const_iterator it = m_sourceLocations.begin();
		it != m_sourceLocations.end(); it++)
	{
		callback(it->second);
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

std::vector<StorageNode> IntermediateStorage::getStorageNodes() const
{
	return m_nodes;
}

std::vector<StorageFile> IntermediateStorage::getStorageFiles() const
{
	return m_files;
}

std::vector<StorageSymbol> IntermediateStorage::getStorageSymbols() const
{
	return m_symbols;
}

std::vector<StorageEdge> IntermediateStorage::getStorageEdges() const
{
	return m_edges;
}

std::vector<StorageLocalSymbol> IntermediateStorage::getStorageLocalSymbols() const
{
	std::vector<StorageLocalSymbol> localSymbol;
	localSymbol.reserve(m_localSymbols.size());
	for (const auto& it: m_localSymbols)
	{
		localSymbol.push_back(it.second);
	}
	return localSymbol;
}

std::vector<StorageSourceLocation> IntermediateStorage::getStorageSourceLocations() const
{
	std::vector<StorageSourceLocation> sourceLocations;
	sourceLocations.reserve(m_sourceLocations.size());
	for (const auto& it: m_sourceLocations)
	{
		sourceLocations.push_back(it.second);
	}
	return sourceLocations;
}

std::vector<StorageOccurrence> IntermediateStorage::getStorageOccurrences() const
{
	return m_occurrences;
}

std::vector<StorageComponentAccess> IntermediateStorage::getComponentAccesses() const
{
	return m_componentAccesses;
}

std::vector<StorageCommentLocation> IntermediateStorage::getCommentLocations() const
{
	return m_commentLocations;
}

std::vector<StorageError> IntermediateStorage::getErrors() const
{
	return m_errors;
}

void IntermediateStorage::setStorageNodes(const std::vector<StorageNode>& storageNodes)
{
	m_nodes.clear();
	m_nodesIndex.clear();
	for (const StorageNode& storageNode: storageNodes)
	{
		m_nodes.push_back(storageNode);
		m_nodesIndex.emplace(serialize(storageNode), m_nodes.size() - 1);
	}
}

void IntermediateStorage::setStorageFiles(const std::vector<StorageFile>& storageFiles)
{
	m_files = storageFiles;
}

void IntermediateStorage::setStorageSymbols(const std::vector<StorageSymbol>& storageSymbols)
{
	m_symbols = storageSymbols;
}

void IntermediateStorage::setStorageEdges(const std::vector<StorageEdge>& storageEdges)
{
	m_edges.clear();
	m_edgesIndex.clear();
	for (const StorageEdge& storageEdge: storageEdges)
	{
		m_edges.push_back(storageEdge);
		m_edgesIndex.emplace(serialize(storageEdge), m_edges.size() - 1);
	}
}

void IntermediateStorage::setStorageLocalSymbols(const std::vector<StorageLocalSymbol>& storageLocalSymbols)
{
	m_localSymbols.clear();
	for (const StorageLocalSymbol& storageLocalSymbol: storageLocalSymbols)
	{
		m_localSymbols[serialize(storageLocalSymbol)] = storageLocalSymbol;
	}
}

void IntermediateStorage::setStorageSourceLocations(const std::vector<StorageSourceLocation>& storageSourceLocations)
{
	m_sourceLocations.clear();
	for (const StorageSourceLocation& storageSourceLocation: storageSourceLocations)
	{
		m_sourceLocations[serialize(storageSourceLocation)] = storageSourceLocation;
	}
}

void IntermediateStorage::setStorageOccurrences(const std::vector<StorageOccurrence>& storageOccurrences)
{
	m_occurrences = storageOccurrences;
}

void IntermediateStorage::setComponentAccesses(const std::vector<StorageComponentAccess>& componentAccesses)
{
	m_componentAccesses = componentAccesses;
}

void IntermediateStorage::setCommentLocations(const std::vector<StorageCommentLocation>& commentLocations)
{
	m_commentLocations = commentLocations;
}

void IntermediateStorage::setErrors(const std::vector<StorageError>& errors)
{
	m_errors = errors;
}

Id IntermediateStorage::getNextId() const
{
	return m_nextId;
}

void IntermediateStorage::setNextId(const Id nextId)
{
	m_nextId = nextId;
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
