#ifndef INDEXER_CXX_CDB_H
#define INDEXER_CXX_CDB_H

#include <vector>

#include "data/indexer/Indexer.h"
#include "data/indexer/IndexerCommandCxxCdb.h"
#include "utility/file/FilePath.h"

class IndexerCxxCdb: public Indexer<IndexerCommandCxxCdb>
{
public:
	static std::vector<FilePath> getSourceFilesFromCDB(const FilePath& compilationDatabasePath);

private:
	virtual std::shared_ptr<IntermediateStorage> index(std::shared_ptr<IndexerCommandCxxCdb> indexerCommand, std::shared_ptr<FileRegister> fileRegister);

};

#endif // INDEXER_CXX_CDB_H
