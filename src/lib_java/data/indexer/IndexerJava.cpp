#include "IndexerJava.h"

#include "IndexerCommandJava.h"
#include "ParserClientImpl.h"
#include "JavaParser.h"

IndexerJava::~IndexerJava()
{
	JavaParser::clearCaches();
}

std::shared_ptr<IntermediateStorage> IndexerJava::doIndex(std::shared_ptr<IndexerCommandJava> indexerCommand)
{
	std::shared_ptr<ParserClientImpl> parserClient = std::make_shared<ParserClientImpl>();

	std::shared_ptr<JavaParser> parser = std::make_shared<JavaParser>(parserClient);

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

	if (interrupted())
	{
		return std::shared_ptr<IntermediateStorage>();
	}

	return storage;
}
