#include "data/indexer/IndexerComposite.h"

#include "data/indexer/IndexerCommand.h"
#include "utility/logging/logging.h"
#include "data/storage/IntermediateStorage.h"

IndexerComposite::~IndexerComposite()
{
}

IndexerCommandType IndexerComposite::getSupportedIndexerCommandType() const
{
	return INDEXER_COMMAND_UNKNOWN;
}

void IndexerComposite::addIndexer(std::shared_ptr<IndexerBase> indexer)
{
	m_indexers.emplace(indexer->getSupportedIndexerCommandType(), indexer);
}

std::shared_ptr<IntermediateStorage> IndexerComposite::index(std::shared_ptr<IndexerCommand> indexerCommand)
{
	auto it = m_indexers.find(indexerCommand->getIndexerCommandType());
	if (it != m_indexers.end())
	{
		return it->second->index(indexerCommand);
	}

	LOG_ERROR("No indexer found that supports \"" + indexerCommandTypeToString(indexerCommand->getIndexerCommandType()) + "\".");
	return std::shared_ptr<IntermediateStorage>();
}

void IndexerComposite::interrupt()
{
	for (auto& it: m_indexers)
	{
		it.second->interrupt();
	}
	IndexerBase::interrupt();
}
