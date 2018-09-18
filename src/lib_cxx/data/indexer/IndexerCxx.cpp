#include "IndexerCxx.h"

#include "CxxParser.h"
#include "ParserClientImpl.h"
#include "FileRegister.h"

std::shared_ptr<IntermediateStorage> IndexerCxx::doIndex(std::shared_ptr<IndexerCommandCxx> indexerCommand)
{
	std::shared_ptr<ParserClientImpl> parserClient = std::make_shared<ParserClientImpl>();

	std::shared_ptr<CxxParser> parser = std::make_shared<CxxParser>(
		parserClient,
		std::make_shared<FileRegister>(
			indexerCommand->getSourceFilePath(), indexerCommand->getIndexedPaths(), indexerCommand->getExcludeFilters()
		)
	);

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
