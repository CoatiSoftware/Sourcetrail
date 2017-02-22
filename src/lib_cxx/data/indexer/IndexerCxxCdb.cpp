#include "data/indexer/IndexerCxxCdb.h"

#include "clang/Tooling/JSONCompilationDatabase.h"
#include "data/parser/ParserClientImpl.h"
#include "data/parser/cxx/CxxParser.h"
#include "utility/file/FileRegister.h"

std::vector<FilePath> IndexerCxxCdb::getSourceFilesFromCDB(const FilePath& compilationDatabasePath)
{
	std::string error;
	std::shared_ptr<clang::tooling::JSONCompilationDatabase> cdb = std::shared_ptr<clang::tooling::JSONCompilationDatabase>
		(clang::tooling::JSONCompilationDatabase::loadFromFile(compilationDatabasePath.str(), error));

	std::vector<FilePath> filePaths;
	if (cdb)
	{
		std::vector<std::string> files = cdb->getAllFiles();
		for (const std::string& file : files)
		{
			filePaths.push_back(FilePath(file));
		}
	}
	return filePaths;
}

std::shared_ptr<IntermediateStorage> IndexerCxxCdb::index(std::shared_ptr<IndexerCommandCxxCdb> indexerCommand, std::shared_ptr<FileRegister> fileRegister)
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
