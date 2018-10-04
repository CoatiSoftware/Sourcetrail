#include "IntermediateStorage.h"

#include "utility.h"

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

std::pair<Id, bool> IntermediateStorage::addNode(const StorageNodeData& nodeData)
{
	auto it = m_nodesIndex.find(nodeData);
	if (it != m_nodesIndex.end())
	{
		StorageNode& storedNode = m_nodes[it->second];
		if (storedNode.type < nodeData.type)
		{
			storedNode.type = nodeData.type;
		}
		return std::make_pair(storedNode.id, false);
	}

	Id nodeId = m_nextId++;
	m_nodes.emplace_back(nodeId, nodeData);
	m_nodesIndex.emplace(nodeData, m_nodes.size() - 1);
	return std::make_pair(nodeId, true);
}

std::vector<Id> IntermediateStorage::addNodes(const std::vector<StorageNode>& nodes)
{
	std::vector<Id> nodeIds;
	nodeIds.reserve(nodes.size());
	for (const StorageNode& node : nodes)
	{
		nodeIds.emplace_back(addNode(node).first);
	}
	return nodeIds;
}

void IntermediateStorage::addSymbol(const StorageSymbol& symbol)
{
	m_symbols.push_back(symbol);
}

void IntermediateStorage::addSymbols(const std::vector<StorageSymbol>& symbols)
{
	m_symbols.insert(m_symbols.end(), symbols.begin(), symbols.end());
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

std::vector<Id> IntermediateStorage::addEdges(const std::vector<StorageEdge>& edges)
{
	std::vector<Id> edgeIds;
	edgeIds.reserve(edges.size());
	for (const StorageEdge& edge : edges)
	{
		edgeIds.emplace_back(addEdge(edge));
	}
	return edgeIds;
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

std::vector<Id> IntermediateStorage::addLocalSymbols(const std::set<StorageLocalSymbol>& symbols)
{
	std::vector<Id> symbolIds;
	symbolIds.reserve(symbols.size());
	for (const StorageLocalSymbol& symbol : symbols)
	{
		symbolIds.emplace_back(addLocalSymbol(symbol));
	}
	return symbolIds;
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

std::vector<Id> IntermediateStorage::addSourceLocations(const std::vector<StorageSourceLocation>& locations)
{
	std::vector<Id> locationIds;
	locationIds.reserve(locations.size());
	for (const StorageSourceLocation& location : locations)
	{
		locationIds.emplace_back(addSourceLocation(location));
	}
	return locationIds;
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

void IntermediateStorage::addComponentAccesses(const std::vector<StorageComponentAccess>& componentAccesses)
{
	m_componentAccesses.insert(componentAccesses.begin(), componentAccesses.end());
}

void IntermediateStorage::addError(const StorageErrorData& errorData)
{
	if (m_errorsIndex.find(errorData) == m_errorsIndex.end())
	{
		m_errors.emplace_back(errorData);
		m_errorsIndex.emplace(errorData);
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
