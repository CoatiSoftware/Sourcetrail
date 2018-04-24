#ifndef INDEXER_CXX_H
#define INDEXER_CXX_H

#include <memory>

#include "data/indexer/Indexer.h"
#include "data/parser/ParserClientImpl.h"
#include "utility/file/FileRegister.h"

template <typename IndexerCommandType, typename ParserType>
class IndexerCxx: public Indexer<IndexerCommandType>
{
public:
	virtual ~IndexerCxx() = default;

	virtual std::shared_ptr<IntermediateStorage> doIndex(
		std::shared_ptr<IndexerCommandType> indexerCommand,
		std::shared_ptr<FileRegister> fileRegister);
};

template <typename IndexerCommandType, typename ParserType>
std::shared_ptr<IntermediateStorage> IndexerCxx<IndexerCommandType, ParserType>::doIndex(
	std::shared_ptr<IndexerCommandType> indexerCommand,
	std::shared_ptr<FileRegister> fileRegister)
{
	std::shared_ptr<ParserClientImpl> parserClient = std::make_shared<ParserClientImpl>();

	std::shared_ptr<ParserType> parser = std::make_shared<ParserType>(parserClient, fileRegister);

	std::shared_ptr<IntermediateStorage> storage = std::make_shared<IntermediateStorage>();
	parserClient->setStorage(storage);

	parser->buildIndex(indexerCommand);

	parserClient->resetStorage();

	if (parserClient->hasFatalErrors())
	{
		storage->setAllFilesIncomplete();
	}
	else
	{
		storage->setFilesWithErrorsIncomplete();
	}

    if (IndexerBase::interrupted())
	{
		return std::shared_ptr<IntermediateStorage>();
	}

	return storage;
}

#endif // INDEXER_CXX_H
