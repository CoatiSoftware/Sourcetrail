#include "data/indexer/IndexerCommandType.h"

std::string indexerCommandTypeToString(IndexerCommandType type)
{
	switch(type)
	{
	case INDEXER_COMMAND_CXX_EMPTY:
		return "indexer command cxx empty";
	case INDEXER_COMMAND_CXX_CDB:
		return "indexer command cxx cdb";
	case INDEXER_COMMAND_JAVA:
		return "indexer command java";
	default:
		break;
	}
	return "indexer command unknown";
}
