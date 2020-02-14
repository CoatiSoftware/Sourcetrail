#include "SharedIntermediateStorage.h"

SharedIntermediateStorage::SharedIntermediateStorage(SharedMemory::Allocator* allocator)
	: m_storageFiles(allocator)
	, m_storageSymbols(allocator)
	, m_storageOccurrences(allocator)
	, m_storageComponentAccesses(allocator)
	, m_storageNodes(allocator)
	, m_storageEdges(allocator)
	, m_storageLocalSymbols(allocator)
	, m_storageSourceLocations(allocator)
	, m_storageErrors(allocator)
	, m_allocator(allocator)
	, m_nextId(1)
{
}

SharedIntermediateStorage::~SharedIntermediateStorage() {}

std::vector<StorageFile> SharedIntermediateStorage::getStorageFiles() const
{
	std::vector<StorageFile> result;
	result.reserve(m_storageFiles.size());

	for (unsigned int i = 0; i < m_storageFiles.size(); i++)
	{
		result.emplace_back(fromShared(m_storageFiles[i]));
	}

	return result;
}

void SharedIntermediateStorage::setStorageFiles(const std::vector<StorageFile>& storageFiles)
{
	m_storageFiles.clear();

	for (unsigned int i = 0; i < storageFiles.size(); i++)
	{
		m_storageFiles.push_back(toShared(storageFiles[i], m_allocator));
	}
}

std::vector<StorageNode> SharedIntermediateStorage::getStorageNodes() const
{
	std::vector<StorageNode> result;
	result.reserve(m_storageNodes.size());

	for (unsigned int i = 0; i < m_storageNodes.size(); i++)
	{
		result.emplace_back(fromShared(m_storageNodes[i]));
	}

	return result;
}

void SharedIntermediateStorage::setStorageNodes(const std::vector<StorageNode>& storageNodes)
{
	m_storageNodes.clear();

	for (unsigned int i = 0; i < storageNodes.size(); i++)
	{
		m_storageNodes.push_back(toShared(storageNodes[i], m_allocator));
	}
}

std::vector<StorageSymbol> SharedIntermediateStorage::getStorageSymbols() const
{
	std::vector<StorageSymbol> result;
	result.reserve(m_storageSymbols.size());

	for (unsigned int i = 0; i < m_storageSymbols.size(); i++)
	{
		result.emplace_back(fromShared(m_storageSymbols[i]));
	}

	return result;
}

void SharedIntermediateStorage::setStorageSymbols(const std::vector<StorageSymbol>& storageSymbols)
{
	m_storageSymbols.clear();

	for (unsigned int i = 0; i < storageSymbols.size(); i++)
	{
		m_storageSymbols.push_back(toShared(storageSymbols[i], m_allocator));
	}
}

std::vector<StorageEdge> SharedIntermediateStorage::getStorageEdges() const
{
	std::vector<StorageEdge> result;
	result.reserve(m_storageEdges.size());

	for (unsigned int i = 0; i < m_storageEdges.size(); i++)
	{
		result.emplace_back(fromShared(m_storageEdges[i]));
	}

	return result;
}

void SharedIntermediateStorage::setStorageEdges(const std::vector<StorageEdge>& storageEdges)
{
	m_storageEdges.clear();

	for (unsigned int i = 0; i < storageEdges.size(); i++)
	{
		m_storageEdges.push_back(toShared(storageEdges[i], m_allocator));
	}
}

std::set<StorageLocalSymbol> SharedIntermediateStorage::getStorageLocalSymbols() const
{
	std::set<StorageLocalSymbol> result;

	for (unsigned int i = 0; i < m_storageLocalSymbols.size(); i++)
	{
		result.emplace(fromShared(m_storageLocalSymbols[i]));
	}

	return result;
}

void SharedIntermediateStorage::setStorageLocalSymbols(
	const std::set<StorageLocalSymbol>& storageLocalSymbols)
{
	m_storageLocalSymbols.clear();

	for (const StorageLocalSymbol& localSymbol: storageLocalSymbols)
	{
		m_storageLocalSymbols.push_back(toShared(localSymbol, m_allocator));
	}
}

std::set<StorageSourceLocation> SharedIntermediateStorage::getStorageSourceLocations() const
{
	std::set<StorageSourceLocation> result;

	for (unsigned int i = 0; i < m_storageSourceLocations.size(); i++)
	{
		result.emplace(fromShared(m_storageSourceLocations[i]));
	}

	return result;
}

void SharedIntermediateStorage::setStorageSourceLocations(
	const std::set<StorageSourceLocation>& storageSourceLocations)
{
	m_storageSourceLocations.clear();

	for (const StorageSourceLocation& sourceLocation: storageSourceLocations)
	{
		m_storageSourceLocations.push_back(toShared(sourceLocation, m_allocator));
	}
}

std::set<StorageOccurrence> SharedIntermediateStorage::getStorageOccurrences() const
{
	std::set<StorageOccurrence> result;

	for (unsigned int i = 0; i < m_storageOccurrences.size(); i++)
	{
		result.emplace(fromShared(m_storageOccurrences[i]));
	}

	return result;
}

void SharedIntermediateStorage::setStorageOccurrences(const std::set<StorageOccurrence>& storageOccurences)
{
	m_storageOccurrences.clear();

	for (const StorageOccurrence& occurrence: storageOccurences)
	{
		m_storageOccurrences.push_back(toShared(occurrence, m_allocator));
	}
}

std::set<StorageComponentAccess> SharedIntermediateStorage::getStorageComponentAccesses() const
{
	std::set<StorageComponentAccess> result;

	for (unsigned int i = 0; i < m_storageComponentAccesses.size(); i++)
	{
		result.emplace(fromShared(m_storageComponentAccesses[i]));
	}

	return result;
}

void SharedIntermediateStorage::setStorageComponentAccesses(
	const std::set<StorageComponentAccess>& storageComponentAccesses)
{
	m_storageComponentAccesses.clear();

	for (const StorageComponentAccess& componentAccess: storageComponentAccesses)
	{
		m_storageComponentAccesses.push_back(toShared(componentAccess, m_allocator));
	}
}

std::vector<StorageError> SharedIntermediateStorage::getStorageErrors() const
{
	std::vector<StorageError> result;
	result.reserve(m_storageErrors.size());

	for (unsigned int i = 0; i < m_storageErrors.size(); i++)
	{
		result.emplace_back(fromShared(m_storageErrors[i]));
	}

	return result;
}

void SharedIntermediateStorage::setStorageErrors(const std::vector<StorageError>& errors)
{
	m_storageErrors.clear();

	for (unsigned int i = 0; i < errors.size(); i++)
	{
		m_storageErrors.push_back(toShared(errors[i], m_allocator));
	}
}

Id SharedIntermediateStorage::getNextId() const
{
	return m_nextId;
}

void SharedIntermediateStorage::setNextId(const Id nextId)
{
	m_nextId = static_cast<int>(nextId);
}
