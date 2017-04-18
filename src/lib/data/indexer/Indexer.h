#ifndef INDEXER_H
#define INDEXER_H

#include <memory>

#include "data/indexer/IndexerBase.h"
#include "data/parser/ParserClientImpl.h"
#include "utility/file/FileRegister.h"
#include "utility/logging/logging.h"

template <typename IndexerCommandType, typename ParserType>
class Indexer
	: public IndexerBase
{
public:
	virtual ~Indexer();

	virtual std::string getKindString() const;

	virtual std::shared_ptr<IntermediateStorage> index(
		std::shared_ptr<IndexerCommand> indexerCommand, std::shared_ptr<FileRegister> fileRegister);
};

template <typename IndexerCommandType, typename ParserType>
Indexer<IndexerCommandType, ParserType>::~Indexer()
{
}

template <typename IndexerCommandType, typename ParserType>
std::string Indexer<IndexerCommandType, ParserType>::getKindString() const
{
	return IndexerCommandType::getIndexerKindString();
}

template <typename IndexerCommandType, typename ParserType>
std::shared_ptr<IntermediateStorage> Indexer<IndexerCommandType, ParserType>::index(
	std::shared_ptr<IndexerCommand> indexerCommand, std::shared_ptr<FileRegister> fileRegister)
{
	std::shared_ptr<IndexerCommandType> castedCommand = std::dynamic_pointer_cast<IndexerCommandType>(indexerCommand);
	if (!castedCommand)
	{
		LOG_ERROR("Trying to process " + indexerCommand->getKindString() +
			" indexer command with " + getKindString() + " indexer.");

		return std::shared_ptr<IntermediateStorage>();
	}

	std::shared_ptr<ParserClientImpl> parserClient = std::make_shared<ParserClientImpl>();
	parserClient->setCancelOnFatalErrors(indexerCommand->cancelOnFatalErrors());

	std::shared_ptr<ParserType> parser = std::make_shared<ParserType>(parserClient, fileRegister);

	std::shared_ptr<IntermediateStorage> storage = std::make_shared<IntermediateStorage>();
	parserClient->setStorage(storage);

	parser->buildIndex(castedCommand);

	parserClient->resetStorage();

	if (parserClient->hasFatalErrors() || indexerCommand->preprocessorOnly())
	{
		storage->setAllFilesIncomplete();
	}
	else
	{
		storage->setFilesWithErrorsIncomplete();
		fileRegister->markIndexingFilesIndexed();
	}

	if (interrupted())
	{
		return std::shared_ptr<IntermediateStorage>();
	}

	return storage;
}

#endif // INDEXER_H
