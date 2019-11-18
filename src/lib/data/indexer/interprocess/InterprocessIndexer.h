#ifndef INTERPROCESS_INDEXER_H
#define INTERPROCESS_INDEXER_H

#include "InterprocessIndexerCommandManager.h"
#include "InterprocessIndexingStatusManager.h"
#include "InterprocessIntermediateStorageManager.h"

class InterprocessIndexer
{
public:
	InterprocessIndexer(const std::string& uuid, Id processId);

	void work();

private:
	InterprocessIndexerCommandManager m_interprocessIndexerCommandManager;
	InterprocessIndexingStatusManager m_interprocessIndexingStatusManager;
	InterprocessIntermediateStorageManager m_interprocessIntermediateStorageManager;

	const std::string m_uuid;
	const Id m_processId;
};

#endif	  // INTERPROCESS_INDEXER_H
