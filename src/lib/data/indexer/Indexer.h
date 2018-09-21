#ifndef INDEXER_H
#define INDEXER_H

#include <memory>

#include "IndexerBase.h"
#include "IndexerCommand.h"
#include "logging.h"

template <typename T>
class Indexer
	: public IndexerBase
{
public:
	IndexerCommandType getSupportedIndexerCommandType() const override;
	std::shared_ptr<IntermediateStorage> index(std::shared_ptr<IndexerCommand> indexerCommand) override;

private:
	virtual std::shared_ptr<IntermediateStorage> doIndex(std::shared_ptr<T> indexerCommand) = 0;
};


template <typename T>
IndexerCommandType Indexer<T>::getSupportedIndexerCommandType() const
{
	return T::getStaticIndexerCommandType();
}

template <typename T>
std::shared_ptr<IntermediateStorage> Indexer<T>::index(std::shared_ptr<IndexerCommand> indexerCommand)
{
	std::shared_ptr<T> castCommand = std::dynamic_pointer_cast<T>(indexerCommand);
	if (!castCommand)
	{
		LOG_ERROR("Trying to process " + indexerCommandTypeToString(indexerCommand->getIndexerCommandType()) +
			" indexer command with indexer that supports \"" + indexerCommandTypeToString(getSupportedIndexerCommandType()) + "\".");

		return std::shared_ptr<IntermediateStorage>();
	}

	return doIndex(castCommand);
}

#endif // INDEXER_H
