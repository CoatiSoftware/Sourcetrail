#ifndef INDEXER_H
#define INDEXER_H

#include <memory>

#include "data/indexer/IndexerBase.h"
#include "data/indexer/IndexerCommand.h"
#include "utility/logging/logging.h"

template <typename IndexerCommandType>
class Indexer
	: public IndexerBase
{
public:
	virtual ~Indexer();

	virtual std::string getKindString() const;

	virtual std::shared_ptr<IntermediateStorage> index(
		std::shared_ptr<IndexerCommand> indexerCommand,
		std::shared_ptr<FileRegister> fileRegister);

	virtual std::shared_ptr<IntermediateStorage> doIndex(
		std::shared_ptr<IndexerCommandType> indexerCommand,
		std::shared_ptr<FileRegister> fileRegister) = 0;
};

template <typename IndexerCommandType>
Indexer<IndexerCommandType>::~Indexer()
{
}

template <typename IndexerCommandType>
std::string Indexer<IndexerCommandType>::getKindString() const
{
	return IndexerCommandType::getIndexerKindString();
}

template <typename IndexerCommandType>
std::shared_ptr<IntermediateStorage> Indexer<IndexerCommandType>::index(
	std::shared_ptr<IndexerCommand> indexerCommand, std::shared_ptr<FileRegister> fileRegister)
{
	std::shared_ptr<IndexerCommandType> castedCommand = std::dynamic_pointer_cast<IndexerCommandType>(indexerCommand);
	if (!castedCommand)
	{
		LOG_ERROR("Trying to process " + indexerCommand->getKindString() +
			" indexer command with " + getKindString() + " indexer.");

		return std::shared_ptr<IntermediateStorage>();
	}

	return doIndex(castedCommand, fileRegister);
}

#endif // INDEXER_H
