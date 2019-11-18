#ifndef INDEXER_H
#define INDEXER_H

#include <memory>

#include "IndexerBase.h"
#include "IndexerCommand.h"
#include "IndexerStateInfo.h"
#include "ParserClientImpl.h"
#include "logging.h"

template <typename T>
class Indexer: public IndexerBase
{
public:
	Indexer();
	IndexerCommandType getSupportedIndexerCommandType() const override;
	std::shared_ptr<IntermediateStorage> index(std::shared_ptr<IndexerCommand> indexerCommand) override;
	void interrupt() override;

private:
	virtual void doIndex(
		std::shared_ptr<T> indexerCommand,
		std::shared_ptr<ParserClientImpl> parserClient,
		std::shared_ptr<IndexerStateInfo> m_indexerStateInfo) = 0;

	std::shared_ptr<IndexerStateInfo> m_indexerStateInfo;
};


template <typename T>
Indexer<T>::Indexer(): m_indexerStateInfo(std::make_shared<IndexerStateInfo>())
{
	m_indexerStateInfo->indexingInterrupted = false;
}

template <typename T>
IndexerCommandType Indexer<T>::getSupportedIndexerCommandType() const
{
	return T::getStaticIndexerCommandType();
}

template <typename T>
void Indexer<T>::interrupt()
{
	m_indexerStateInfo->indexingInterrupted = true;
}

template <typename T>
std::shared_ptr<IntermediateStorage> Indexer<T>::index(std::shared_ptr<IndexerCommand> indexerCommand)
{
	std::shared_ptr<T> castCommand = std::dynamic_pointer_cast<T>(indexerCommand);
	if (!castCommand)
	{
		LOG_ERROR(
			"Trying to process " +
			indexerCommandTypeToString(indexerCommand->getIndexerCommandType()) +
			" indexer command with indexer that supports \"" +
			indexerCommandTypeToString(getSupportedIndexerCommandType()) + "\".");

		return nullptr;
	}

	std::shared_ptr<IntermediateStorage> storage = std::make_shared<IntermediateStorage>();
	std::shared_ptr<ParserClientImpl> parserClient = std::make_shared<ParserClientImpl>(storage.get());

	doIndex(castCommand, parserClient, m_indexerStateInfo);

	if (storage->hasFatalErrors())
	{
		storage->setAllFilesIncomplete();
	}
	else
	{
		storage->setFilesWithErrorsIncomplete();
	}

	if (m_indexerStateInfo->indexingInterrupted)
	{
		return nullptr;
	}

	return storage;
}

#endif	  // INDEXER_H
