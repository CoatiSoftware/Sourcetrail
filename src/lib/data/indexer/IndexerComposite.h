#ifndef INDEXER_COMPOSITE_H
#define INDEXER_COMPOSITE_H

#include <memory>
#include <unordered_map>

#include "data/indexer/IndexerBase.h"

class IndexerComposite: public IndexerBase
{
public:
	virtual ~IndexerComposite();

	virtual IndexerCommandType getSupportedIndexerCommandType() const;

	void addIndexer(std::shared_ptr<IndexerBase> indexer);

	virtual std::shared_ptr<IntermediateStorage> index(
		std::shared_ptr<IndexerCommand> indexerCommand,
		std::shared_ptr<FileRegister> fileRegister
	);

	virtual void interrupt();

private:
	std::unordered_map<IndexerCommandType, std::shared_ptr<IndexerBase>> m_indexers;
};

#endif // INDEXER_COMPOSITE_H
