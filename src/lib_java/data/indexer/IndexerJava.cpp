#include "data/indexer/IndexerJava.h"

#include "data/parser/ParserClientImpl.h"
#include "data/parser/java/JavaParser.h"
#include "utility/file/FileRegister.h"

IndexerJava::IndexerJava()
{
}

IndexerJava::~IndexerJava()
{
}

std::shared_ptr<IntermediateStorage> IndexerJava::index(std::shared_ptr<IndexerCommandJava> indexerCommand, std::shared_ptr<FileRegister> fileRegister)
{
	std::shared_ptr<ParserClientImpl> parserClient = std::make_shared<ParserClientImpl>();
	std::shared_ptr<JavaParser> parser = std::make_shared<JavaParser>(parserClient);

	std::shared_ptr<IntermediateStorage> storage = std::make_shared<IntermediateStorage>();
	parserClient->setStorage(storage);
	parserClient->startParsingFile();

	fileRegister->markFileIndexing(indexerCommand->getSourceFilePath());
	parser->buildIndex(indexerCommand);
	fileRegister->markIndexingFilesIndexed();

	parserClient->finishParsingFile();
	parserClient->resetStorage();

	if (interrupted())
	{
		return std::shared_ptr<IntermediateStorage>();
	}

	return storage;
}
