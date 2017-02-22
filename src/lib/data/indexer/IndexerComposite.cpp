#include "data/indexer/IndexerComposite.h"
#include "utility/logging/logging.h"

IndexerComposite::~IndexerComposite()
{
}

std::string IndexerComposite::getKindString() const
{
	return "composite";
}

void IndexerComposite::addIndexer(std::shared_ptr<IndexerBase> indexer)
{
	m_indexers.emplace(indexer->getKindString(), indexer);
}

std::shared_ptr<IntermediateStorage> IndexerComposite::index(std::shared_ptr<IndexerCommand> indexerCommand, std::shared_ptr<FileRegister> fileRegister)
{
	auto it = m_indexers.find(indexerCommand->getKindString());
	if (it != m_indexers.end())
	{
		return it->second->index(indexerCommand, fileRegister);
	}

	LOG_ERROR("No indexer found to handle " + indexerCommand->getKindString() + " indexer command.");
	return std::shared_ptr<IntermediateStorage>();
}

void IndexerComposite::interrupt()
{
	for (auto it: m_indexers)
	{
		it.second->interrupt();
	}
	IndexerBase::interrupt();
}
