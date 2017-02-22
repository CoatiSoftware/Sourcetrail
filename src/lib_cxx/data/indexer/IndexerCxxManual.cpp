#include "data/indexer/IndexerCxxManual.h"

#include "data/parser/ParserClientImpl.h"
#include "data/parser/cxx/CxxParser.h"
#include "utility/file/FileRegister.h"

std::shared_ptr<IntermediateStorage> IndexerCxxManual::index(std::shared_ptr<IndexerCommandCxxManual> indexerCommand, std::shared_ptr<FileRegister> fileRegister)
{
	std::shared_ptr<ParserClientImpl> parserClient = std::make_shared<ParserClientImpl>();
	std::shared_ptr<CxxParser> parser = std::make_shared<CxxParser>(parserClient, fileRegister);

	std::shared_ptr<IntermediateStorage> storage = std::make_shared<IntermediateStorage>();
	parserClient->setStorage(storage);
	parserClient->startParsingFile();

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
