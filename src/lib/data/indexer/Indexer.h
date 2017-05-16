#ifndef INDEXER_H
#define INDEXER_H

#include <memory>

#include "data/indexer/IndexerBase.h"
#include "data/indexer/IndexerCommand.h"
#include "utility/logging/logging.h"

template <typename T>
class Indexer
	: public IndexerBase
{
public:
	virtual ~Indexer();

	virtual IndexerCommandType getSupportedIndexerCommandType() const;

	virtual std::shared_ptr<IntermediateStorage> index(
		std::shared_ptr<IndexerCommand> indexerCommand,
		std::shared_ptr<FileRegister> fileRegister);

	virtual std::shared_ptr<IntermediateStorage> doIndex(
		std::shared_ptr<T> indexerCommand,
		std::shared_ptr<FileRegister> fileRegister) = 0;
};

template <typename T>
Indexer<T>::~Indexer()
{
}

template <typename T>
IndexerCommandType Indexer<T>::getSupportedIndexerCommandType() const
{
	return T::getStaticIndexerCommandType();
}

template <typename T>
std::shared_ptr<IntermediateStorage> Indexer<T>::index(
	std::shared_ptr<IndexerCommand> indexerCommand, std::shared_ptr<FileRegister> fileRegister)
{
	std::shared_ptr<T> castedCommand = std::dynamic_pointer_cast<T>(indexerCommand);
	if (!castedCommand)
	{
		LOG_ERROR("Trying to process " + indexerCommandTypeToString(indexerCommand->getIndexerCommandType()) +
			" indexer command with indexer that supports \"" + indexerCommandTypeToString(getSupportedIndexerCommandType()) + "\".");

		return std::shared_ptr<IntermediateStorage>();
	}

	return doIndex(castedCommand, fileRegister);
}

#endif // INDEXER_H
