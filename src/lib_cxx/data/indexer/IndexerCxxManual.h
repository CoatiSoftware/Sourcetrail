#ifndef INDEXER_CXX_MANUAL_H
#define INDEXER_CXX_MANUAL_H

#include "data/indexer/Indexer.h"
#include "data/indexer/IndexerCommandCxxManual.h"

class IndexerCxxManual: public Indexer<IndexerCommandCxxManual>
{
private:
	virtual std::shared_ptr<IntermediateStorage> index(std::shared_ptr<IndexerCommandCxxManual> indexerCommand, std::shared_ptr<FileRegister> fileRegister);
};

#endif // INDEXER_CXX_MANUAL_H
