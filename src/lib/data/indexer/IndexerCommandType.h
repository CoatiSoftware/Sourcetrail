#ifndef INDEXER_COMMAND_TYPE_H
#define INDEXER_COMMAND_TYPE_H

#include <string>

enum IndexerCommandType
{
	INDEXER_COMMAND_UNKNOWN,
	INDEXER_COMMAND_CXX_EMPTY,
	INDEXER_COMMAND_CXX_CDB,
	INDEXER_COMMAND_JAVA,
};

std::string indexerCommandTypeToString(IndexerCommandType type);
IndexerCommandType stringToIndexerCommandType(const std::string& s);

#endif // INDEXER_COMMAND_TYPE_H
