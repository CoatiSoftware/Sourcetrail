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

size_t IntermediateStorage::getByteSize(size_t stringSize) const
{
	unsigned int byteSize = 0;

	for (const StorageFile& storageFile: getStorageFiles())
	{
		byteSize += sizeof(StorageFile);
		byteSize += stringSize + storageFile.filePath.size();
		byteSize += stringSize + storageFile.modificationTime.size();
	}

	for (const StorageErrorData& storageError: getErrors())
	{
		byteSize += sizeof(StorageErrorData);
		byteSize += stringSize + storageError.filePath.str().size();
		byteSize += stringSize + storageError.message.size();
	}

	for (const StorageNode& storageNode: getStorageNodes())
	{
		byteSize += sizeof(StorageNode);
		byteSize += stringSize + storageNode.serializedName.size();
	}

	for (const StorageLocalSymbol& storageLocalSymbol: getStorageLocalSymbols())
	{
		byteSize += sizeof(StorageLocalSymbol);
		byteSize += stringSize + storageLocalSymbol.name.size();
	}

	byteSize += sizeof(StorageEdge) * getStorageEdges().size();
	byteSize += sizeof(StorageCommentLocationData) * getCommentLocations().size();
	byteSize += sizeof(StorageComponentAccessData) * getComponentAccesses().size();
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
	for (const StorageErrorData& error : m_errors)
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

Id IntermediateStorage::addNode(const StorageNodeData& nodeData)
{

	const std::string serialized = serialize(nodeData);
	std::unordered_map<std::string, size_t>::iterator it = m_nodesIndex.find(serialized);
	if (it != m_nodesIndex.end())
	{
		StorageNode& storedNode = m_nodes[it->second];
		if (storedNode.type < nodeData.type)
		{
			storedNode.type = nodeData.type;
		}
		return storedNode.id;
	}

	const StorageNode node(m_nextId++, nodeData);
	m_nodes.push_back(node);
	m_nodesIndex.emplace(serialized, m_nodes.size() - 1);
	return node.id;
}

void IntermediateStorage::addSymbol(const StorageSymbol& symbol)
{
	m_symbols.push_back(symbol);
}

void IntermediateStorage::addFile(const StorageFile& file)
{
	const std::string serialized = serialize(file);
	if (m_serializedFiles.find(serialized) == m_serializedFiles.end())
	{
		m_files.push_back(file);
		m_serializedFiles.insert(serialized);

	}
}

Id IntermediateStorage::addEdge(const StorageEdgeData& edgeData)
{

	const std::string serialized = serialize(edgeData);
	std::unordered_map<std::string, size_t>::const_iterator it = m_edgesIndex.find(serialized);
	if (it != m_edgesIndex.end())
	{
		return m_edges[it->second].id;
	}

	const StorageEdge edge = StorageEdge(m_nextId++, edgeData);
	m_edges.push_back(edge);
	m_edgesIndex.emplace(serialized, m_edges.size() - 1);
	return edge.id;
}

Id IntermediateStorage::addLocalSymbol(const StorageLocalSymbolData& localSymbolData)
{

	const std::string serialized = serialize(localSymbolData);
	std::unordered_map<std::string, StorageLocalSymbol>::const_iterator it = m_localSymbols.find(serialized);
	if (it != m_localSymbols.end())
	{
		return it->second.id;
	}

	const StorageLocalSymbol localSymbol = StorageLocalSymbol(m_nextId++, localSymbolData);
	m_localSymbols[serialized] = localSymbol;
	return localSymbol.id;
}

Id IntermediateStorage::addSourceLocation(const StorageSourceLocationData& sourceLocationData)
{
	const std::string serialized = serialize(sourceLocationData);
	std::unordered_map<std::string, StorageSourceLocation>::const_iterator it = m_sourceLocations.find(serialized);
	if (it != m_sourceLocations.end())
	{
		return it->second.id;
	}

	const StorageSourceLocation sourceLocation = StorageSourceLocation(m_nextId++, sourceLocationData);
	m_sourceLocations[serialized] = sourceLocation;
	return sourceLocation.id;
}

void IntermediateStorage::addOccurrence(const StorageOccurrence& occurrence)
{
	const std::string serialized = serialize(occurrence);

	if (m_serializedOccurrences.find(serialized) == m_serializedOccurrences.end())
	{
		m_occurrences.push_back(occurrence);
		m_serializedOccurrences.insert(serialized);
	}
}

void IntermediateStorage::addComponentAccess(const StorageComponentAccessData& componentAccessData)
{
	const std::string serialized = serialize(componentAccessData);

	if (m_serializedComponentAccesses.find(serialized) == m_serializedComponentAccesses.end())
	{
		m_componentAccesses.push_back(componentAccessData);
		m_serializedComponentAccesses.insert(serialized);
	}
}

void IntermediateStorage::addCommentLocation(const StorageCommentLocationData& commentLocationData)
{
	const std::string serialized = serialize(commentLocationData);

	if (m_serializedCommentLocations.find(serialized) == m_serializedCommentLocations.end())
	{
		m_commentLocations.push_back(commentLocationData);
		m_serializedCommentLocations.insert(serialized);
	}
}

void IntermediateStorage::addError(const StorageErrorData& errorData)
{
	const std::string serialized = serialize(errorData);

	if (m_serializedErrors.find(serialized) == m_serializedErrors.end())
	{
		m_errors.push_back(errorData);
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

void IntermediateStorage::forEachComponentAccess(std::function<void(const StorageComponentAccessData& /*data*/)> callback) const
{
	for (std::vector<StorageComponentAccessData>::const_iterator it = m_componentAccesses.begin(); it != m_componentAccesses.end(); it++)
	{
		callback(*it);
	}
}

void IntermediateStorage::forEachCommentLocation(std::function<void(const StorageCommentLocationData& /*data*/)> callback) const
{
	for (std::vector<StorageCommentLocationData>::const_iterator it = m_commentLocations.begin(); it != m_commentLocations.end(); it++)
	{
		callback(*it);
	}
}

void IntermediateStorage::forEachError(std::function<void(const StorageErrorData& /*data*/)> callback) const
{
	for (std::vector<StorageErrorData>::const_iterator it = m_errors.begin(); it != m_errors.end(); it++)
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

std::vector<StorageComponentAccessData> IntermediateStorage::getComponentAccesses() const
{
	return m_componentAccesses;
}

std::vector<StorageCommentLocationData> IntermediateStorage::getCommentLocations() const
{
	return m_commentLocations;
}

std::vector<StorageErrorData> IntermediateStorage::getErrors() const
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

void IntermediateStorage::setComponentAccesses(const std::vector<StorageComponentAccessData>& componentAccesses)
{
	m_componentAccesses = componentAccesses;
}

void IntermediateStorage::setCommentLocations(const std::vector<StorageCommentLocationData>& commentLocations)
{
	m_commentLocations = commentLocations;
}

void IntermediateStorage::setErrors(const std::vector<StorageErrorData>& errors)
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

std::string IntermediateStorage::serialize(const StorageNodeData& nodeData) const
{
	return nodeData.serializedName;
}

std::string IntermediateStorage::serialize(const StorageFile& file) const
{
	return file.filePath;
}

std::string IntermediateStorage::serialize(const StorageEdgeData& edgeData) const
{
	return (
		std::to_string(edgeData.type) + ";" +
		std::to_string(edgeData.sourceNodeId) + ";" +
		std::to_string(edgeData.targetNodeId)
	);
}

std::string IntermediateStorage::serialize(const StorageLocalSymbolData& localSymbolData) const
{
	return localSymbolData.name;
}

std::string IntermediateStorage::serialize(const StorageSourceLocationData& sourceLocationData) const
{
	return (
		std::to_string(sourceLocationData.fileNodeId) + ";" +
		std::to_string(sourceLocationData.startLine) + ";" +
		std::to_string(sourceLocationData.startCol) + ";" +
		std::to_string(sourceLocationData.endLine) + ";" +
		std::to_string(sourceLocationData.endCol) + ";" +
		std::to_string(sourceLocationData.type)
	);
}

std::string IntermediateStorage::serialize(const StorageOccurrence& occurrence) const
{
	return std::to_string(occurrence.elementId) +  ";" + std::to_string(occurrence.sourceLocationId);
}


std::string IntermediateStorage::serialize(const StorageComponentAccessData& componentAccessData) const
{
	return std::to_string(componentAccessData.nodeId);
}


std::string IntermediateStorage::serialize(const StorageCommentLocationData& commentLocationData) const
{
	return (
		std::to_string(commentLocationData.fileNodeId) + ";" +
		std::to_string(commentLocationData.startLine) + ";" +
		std::to_string(commentLocationData.startCol) + ";" +
		std::to_string(commentLocationData.endLine) + ";" +
		std::to_string(commentLocationData.endCol)
	);
}


std::string IntermediateStorage::serialize(const StorageErrorData& errorData) const
{
	return (
		errorData.message + ";" +
		std::to_string(errorData.fatal) + ";" +
		errorData.filePath.str() + ";" +
		std::to_string(errorData.lineNumber) + ";" +
		std::to_string(errorData.columnNumber)
	);
}
