#include "IndexerJava.h"

#include "IndexerCommandJava.h"
#include "IndexerStateInfo.h"
#include "JavaParser.h"
#include "ParserClientImpl.h"

IndexerJava::IndexerJava()
	: m_indexerStateInfo(std::make_shared<IndexerStateInfo>())
{
	m_indexerStateInfo->indexingInterrupted = false;
}

IndexerJava::~IndexerJava()
{
	JavaParser::clearCaches();
}

void IndexerJava::interrupt()
{
	m_indexerStateInfo->indexingInterrupted = true;
}

std::shared_ptr<IntermediateStorage> IndexerJava::doIndex(std::shared_ptr<IndexerCommandJava> indexerCommand)
{
	std::shared_ptr<ParserClientImpl> parserClient = std::make_shared<ParserClientImpl>();

	std::shared_ptr<JavaParser> parser = std::make_shared<JavaParser>(parserClient, m_indexerStateInfo);

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
