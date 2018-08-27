#include "data/storage/IntermediateStorage.h"

#include "utility/utility.h"

IntermediateStorage::IntermediateStorage()
	: m_nextId(1)
{
}

void IntermediateStorage::clear()
{
	m_nodesIndex.clear();
	m_nodes.clear();

	m_filesIndex.clear();
	m_files.clear();

	m_symbols.clear();

	m_edgesIndex.clear();
	m_edges.clear();

	m_localSymbols.clear();
	m_sourceLocations.clear();
	m_occurrences.clear();
	m_componentAccesses.clear();
	m_commentLocations.clear();

	m_errorsIndex.clear();
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
		byteSize += stringSize + storageError.filePath.size();
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
	std::set<std::wstring> errorFileNames;
	for (const StorageErrorData& error : m_errors)
	{
		errorFileNames.insert(error.filePath);
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
	auto it = m_nodesIndex.find(nodeData);
	if (it != m_nodesIndex.end())
	{
		StorageNode& storedNode = m_nodes[it->second];
		if (storedNode.type < nodeData.type)
		{
			storedNode.type = nodeData.type;
		}
		return storedNode.id;
	}

	Id nodeId = m_nextId++;
	m_nodes.emplace_back(nodeId, nodeData);
	m_nodesIndex.emplace(nodeData, m_nodes.size() - 1);
	return nodeId;
}

void IntermediateStorage::addSymbol(const StorageSymbol& symbol)
{
	m_symbols.push_back(symbol);
}

void IntermediateStorage::addFile(const StorageFile& file)
{
	auto it = m_filesIndex.find(file);
	if (it == m_filesIndex.end())
	{
		m_filesIndex.emplace(file, m_files.size());
		m_files.emplace_back(file);
	}
	else
	{
		StorageFile& storedFile = m_files[it->second];

		if (file.indexed)
		{
			storedFile.indexed = true;
		}

		if (file.complete)
		{
			storedFile.complete = true;
		}
	}
}

Id IntermediateStorage::addEdge(const StorageEdgeData& edgeData)
{
	auto it = m_edgesIndex.find(edgeData);
	if (it != m_edgesIndex.end())
	{
		return m_edges[it->second].id;
	}

	Id edgeId = m_nextId++;
	m_edges.emplace_back(edgeId, edgeData);
	m_edgesIndex.emplace(edgeData, m_edges.size() - 1);
	return edgeId;
}

Id IntermediateStorage::addLocalSymbol(const StorageLocalSymbolData& localSymbolData)
{
	auto it = m_localSymbols.find(StorageLocalSymbol(0, localSymbolData));
	if (it != m_localSymbols.end())
	{
		return it->id;
	}

	Id localSymbolId = m_nextId++;
	m_localSymbols.emplace(localSymbolId, localSymbolData);
	return localSymbolId;
}

Id IntermediateStorage::addSourceLocation(const StorageSourceLocationData& sourceLocationData)
{
	auto it = m_sourceLocations.find(StorageSourceLocation(0, sourceLocationData));
	if (it != m_sourceLocations.end())
	{
		return it->id;
	}

	Id sourceLocationId = m_nextId++;
	m_sourceLocations.emplace(sourceLocationId, sourceLocationData);
	return sourceLocationId;
}

void IntermediateStorage::addOccurrence(const StorageOccurrence& occurrence)
{
	m_occurrences.emplace(occurrence);
}

void IntermediateStorage::addOccurrences(const std::vector<StorageOccurrence>& occurrences)
{
	m_occurrences.insert(occurrences.begin(), occurrences.end());
}

void IntermediateStorage::addComponentAccess(const StorageComponentAccess& componentAccess)
{
	m_componentAccesses.emplace(componentAccess);
}

void IntermediateStorage::addCommentLocation(const StorageCommentLocationData& commentLocationData)
{
	m_commentLocations.emplace(commentLocationData);
}

void IntermediateStorage::addError(const StorageErrorData& errorData)
{
	if (m_errorsIndex.find(errorData) == m_errorsIndex.end())
	{
		m_errors.emplace_back(errorData);
		m_errorsIndex.emplace(errorData);
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
	for (const StorageFile& file : m_files)
	{
		callback(file);
	}
}

void IntermediateStorage::forEachSymbol(std::function<void(const StorageSymbol& /*data*/)> callback) const
{
	for (const StorageSymbol& symbol : m_symbols)
	{
		callback(symbol);
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
	for (const StorageLocalSymbol& localSymbol : m_localSymbols)
	{
		callback(localSymbol);
	}
}

void IntermediateStorage::forEachSourceLocation(std::function<void(const StorageSourceLocation& /*data*/)> callback) const
{
	for (const StorageSourceLocation& sourceLocation : m_sourceLocations)
	{
		callback(sourceLocation);
	}
}

void IntermediateStorage::forEachOccurrence(std::function<void(const StorageOccurrence& /*data*/)> callback) const
{
	for (const StorageOccurrence& occurrence : m_occurrences)
	{
		callback(occurrence);
	}
}

void IntermediateStorage::forEachComponentAccess(std::function<void(const StorageComponentAccess& /*data*/)> callback) const
{
	for (const StorageComponentAccess& componentAccess : m_componentAccesses)
	{
		callback(componentAccess);
	}
}

void IntermediateStorage::forEachCommentLocation(std::function<void(const StorageCommentLocationData& /*data*/)> callback) const
{
	for (const StorageCommentLocationData& commentLocation : m_commentLocations)
	{
		callback(commentLocation);
	}
}

void IntermediateStorage::forEachError(std::function<void(const StorageErrorData& /*data*/)> callback) const
{
	for (const StorageErrorData& error : m_errors)
	{
		callback(error);
	}
}

const std::vector<StorageNode>& IntermediateStorage::getStorageNodes() const
{
	return m_nodes;
}

const std::vector<StorageFile>& IntermediateStorage::getStorageFiles() const
{
	return m_files;
}

const std::vector<StorageSymbol>& IntermediateStorage::getStorageSymbols() const
{
	return m_symbols;
}

const std::vector<StorageEdge>& IntermediateStorage::getStorageEdges() const
{
	return m_edges;
}

const std::set<StorageLocalSymbol>& IntermediateStorage::getStorageLocalSymbols() const
{
	return m_localSymbols;
}

const std::set<StorageSourceLocation>& IntermediateStorage::getStorageSourceLocations() const
{
	return m_sourceLocations;
}

const std::set<StorageOccurrence>& IntermediateStorage::getStorageOccurrences() const
{
	return m_occurrences;
}

const std::set<StorageComponentAccess>& IntermediateStorage::getComponentAccesses() const
{
	return m_componentAccesses;
}

const std::set<StorageCommentLocationData>& IntermediateStorage::getCommentLocations() const
{
	return m_commentLocations;
}

const std::vector<StorageErrorData>& IntermediateStorage::getErrors() const
{
	return m_errors;
}

void IntermediateStorage::setStorageNodes(std::vector<StorageNode> storageNodes)
{
	m_nodes = std::move(storageNodes);

	m_nodesIndex.clear();
	for (size_t i = 0; i < m_nodes.size(); i++)
	{
		m_nodesIndex.emplace(m_nodes[i], i);
	}
}

void IntermediateStorage::setStorageFiles(std::vector<StorageFile> storageFiles)
{
	m_files = std::move(storageFiles);

	m_filesIndex.clear();
	for (size_t i = 0; i < m_files.size(); i++)
	{
		m_filesIndex.emplace(m_files[i], i);
	}
}

void IntermediateStorage::setStorageSymbols(std::vector<StorageSymbol> storageSymbols)
{
	m_symbols = std::move(storageSymbols);
}

void IntermediateStorage::setStorageEdges(std::vector<StorageEdge> storageEdges)
{
	m_edges = std::move(storageEdges);

	m_edgesIndex.clear();
	for (size_t i = 0; i < m_edges.size(); i++)
	{
		m_edgesIndex.emplace(m_edges[i], i);
	}
}

void IntermediateStorage::setStorageLocalSymbols(std::set<StorageLocalSymbol> storageLocalSymbols)
{
	m_localSymbols = std::move(storageLocalSymbols);
}

void IntermediateStorage::setStorageSourceLocations(std::set<StorageSourceLocation> storageSourceLocations)
{
	m_sourceLocations = std::move(storageSourceLocations);
}

void IntermediateStorage::setStorageOccurrences(std::set<StorageOccurrence> storageOccurrences)
{
	m_occurrences = std::move(storageOccurrences);
}

void IntermediateStorage::setComponentAccesses(std::set<StorageComponentAccess> componentAccesses)
{
	m_componentAccesses = std::move(componentAccesses);
}

void IntermediateStorage::setCommentLocations(std::set<StorageCommentLocationData> commentLocations)
{
	m_commentLocations = std::move(commentLocations);
}

void IntermediateStorage::setErrors(std::vector<StorageErrorData> errors)
{
	m_errors = std::move(errors);
	m_errorsIndex = utility::toSet(m_errors);
}

Id IntermediateStorage::getNextId() const
{
	return m_nextId;
}

void IntermediateStorage::setNextId(const Id nextId)
{
	m_nextId = nextId;
}
