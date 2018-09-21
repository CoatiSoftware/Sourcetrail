#include "IndexerCxx.h"

#include "CxxParser.h"
#include "FileRegister.h"
#include "IndexerStateInfo.h"
#include "ParserClientImpl.h"

IndexerCxx::IndexerCxx()
	: m_indexerStateInfo(std::make_shared<IndexerStateInfo>())
{
	m_indexerStateInfo->indexingInterrupted = false;
}

void IndexerCxx::interrupt()
{
	m_indexerStateInfo->indexingInterrupted = true;
}

std::shared_ptr<IntermediateStorage> IndexerCxx::doIndex(std::shared_ptr<IndexerCommandCxx> indexerCommand)
{
	std::shared_ptr<ParserClientImpl> parserClient = std::make_shared<ParserClientImpl>();

	std::shared_ptr<CxxParser> parser = std::make_shared<CxxParser>(
		parserClient,
		std::make_shared<FileRegister>(
			indexerCommand->getSourceFilePath(), indexerCommand->getIndexedPaths(), indexerCommand->getExcludeFilters()
		),
		m_indexerStateInfo
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

    if (m_indexerStateInfo->indexingInterrupted)
	{
		return std::shared_ptr<IntermediateStorage>();
	}

	return storage;
}
