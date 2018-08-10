#include "SharedIntermediateStorage.h"

SharedIntermediateStorage::SharedIntermediateStorage(SharedMemory::Allocator* allocator)
	: m_storageFiles(allocator)
	, m_storageSymbols(allocator)
	, m_storageOccurrences(allocator)
	, m_storageComponentAccesses(allocator)
	, m_storageCommentLocations(allocator)
	, m_storageNodes(allocator)
	, m_storageEdges(allocator)
	, m_storageLocalSymbols(allocator)
	, m_storageSourceLocations(allocator)
	, m_storageErrors(allocator)
	, m_allocator(allocator)
	, m_nextId(1)
{
}

SharedIntermediateStorage::~SharedIntermediateStorage()
{
}

std::vector<StorageFile> SharedIntermediateStorage::getStorageFiles() const
{
	std::vector<StorageFile> result;

	for (unsigned int i = 0; i < m_storageFiles.size(); i++)
	{
		result.push_back(fromShared(m_storageFiles[i]));
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

	for (unsigned int i = 0; i < m_storageNodes.size(); i++)
	{
		result.push_back(fromShared(m_storageNodes[i]));
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

	for (unsigned int i = 0; i < m_storageSymbols.size(); i++)
	{
		result.push_back(fromShared(m_storageSymbols[i]));
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

	for (unsigned int i = 0; i < m_storageEdges.size(); i++)
	{
		result.push_back(fromShared(m_storageEdges[i]));
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

std::vector<StorageLocalSymbol> SharedIntermediateStorage::getStorageLocalSymbols() const
{
	std::vector<StorageLocalSymbol> result;

	for (unsigned int i = 0; i < m_storageLocalSymbols.size(); i++)
	{
		result.push_back(fromShared(m_storageLocalSymbols[i]));
	}

	return result;
}

void SharedIntermediateStorage::setStorageLocalSymbols(const std::vector<StorageLocalSymbol>& storageLocalSymbols)
{
	m_storageLocalSymbols.clear();

	for (unsigned int i = 0; i < storageLocalSymbols.size(); i++)
	{
		m_storageLocalSymbols.push_back(toShared(storageLocalSymbols[i], m_allocator));
	}
}

std::vector<StorageSourceLocation> SharedIntermediateStorage::getStorageSourceLocations() const
{
	std::vector<StorageSourceLocation> result;

	for (unsigned int i = 0; i < m_storageSourceLocations.size(); i++)
	{
		result.push_back(fromShared(m_storageSourceLocations[i]));
	}

	return result;
}

void SharedIntermediateStorage::setStorageSourceLocations(const std::vector<StorageSourceLocation>& storageSourceLocations)
{
	m_storageSourceLocations.clear();

	for (unsigned int i = 0; i < storageSourceLocations.size(); i++)
	{
		m_storageSourceLocations.push_back(toShared(storageSourceLocations[i], m_allocator));
	}
}

std::vector<StorageOccurrence> SharedIntermediateStorage::getStorageOccurrences() const
{
	std::vector<StorageOccurrence> result;

	for (unsigned int i = 0; i < m_storageOccurrences.size(); i++)
	{
		result.push_back(fromShared(m_storageOccurrences[i]));
	}

	return result;
}

void SharedIntermediateStorage::setStorageOccurrences(const std::vector<StorageOccurrence>& storageOccurences)
{
	m_storageOccurrences.clear();

	for (unsigned int i = 0; i < storageOccurences.size(); i++)
	{
		m_storageOccurrences.push_back(toShared(storageOccurences[i], m_allocator));
	}
}

std::vector<StorageComponentAccess> SharedIntermediateStorage::getStorageComponentAccesses() const
{
	std::vector<StorageComponentAccess> result;

	for (unsigned int i = 0; i < m_storageComponentAccesses.size(); i++)
	{
		result.push_back(fromShared(m_storageComponentAccesses[i]));
	}

	return result;
}

void SharedIntermediateStorage::setStorageComponentAccesses(const std::vector<StorageComponentAccess>& storageComponentAccesses)
{
	m_storageComponentAccesses.clear();

	for (unsigned int i = 0; i < storageComponentAccesses.size(); i++)
	{
		m_storageComponentAccesses.push_back(toShared(storageComponentAccesses[i], m_allocator));
	}
}

std::vector<StorageCommentLocationData> SharedIntermediateStorage::getStorageCommentLocations() const
{
	std::vector<StorageCommentLocationData> result;

	for (unsigned int i = 0; i < m_storageCommentLocations.size(); i++)
	{
		result.push_back(fromShared(m_storageCommentLocations[i]));
	}

	return result;
}

void SharedIntermediateStorage::setStorageCommentLocations(const std::vector<StorageCommentLocationData>& commentLocations)
{
	m_storageCommentLocations.clear();

	for (unsigned int i = 0; i < commentLocations.size(); i++)
	{
		m_storageCommentLocations.push_back(toShared(commentLocations[i], m_allocator));
	}
}

std::vector<StorageErrorData> SharedIntermediateStorage::getStorageErrors() const
{
	std::vector<StorageErrorData> result;

	for (unsigned int i = 0; i < m_storageErrors.size(); i++)
	{
		result.push_back(fromShared(m_storageErrors[i]));
	}

	return result;
}

void SharedIntermediateStorage::setStorageErrors(const std::vector<StorageErrorData>& errors)
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
	m_nextId = nextId;
}
