#include "data/indexer/IndexerCommandType.h"

std::string indexerCommandTypeToString(IndexerCommandType type)
{
	switch(type)
	{
	case INDEXER_COMMAND_CXX_EMPTY:
		return "indexer_command_cxx_empty";
	case INDEXER_COMMAND_CXX_CDB:
		return "indexer_command_cxx_cdb";
	case INDEXER_COMMAND_JAVA:
		return "indexer_command_java";
	default:
		break;
	}
	return "indexer_command_unknown";
}

IndexerCommandType stringToIndexerCommandType(const std::string& s)
{
	if (s == indexerCommandTypeToString(INDEXER_COMMAND_CXX_EMPTY)) return INDEXER_COMMAND_CXX_EMPTY;
	if (s == indexerCommandTypeToString(INDEXER_COMMAND_CXX_CDB)) return INDEXER_COMMAND_CXX_CDB;
	if (s == indexerCommandTypeToString(INDEXER_COMMAND_JAVA)) return INDEXER_COMMAND_JAVA;
	return INDEXER_COMMAND_UNKNOWN;
}

