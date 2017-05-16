#include "data/indexer/IndexerCommandType.h"

std::string indexerCommandTypeToString(IndexerCommandType type)
{
	switch(type)
	{
	case INDEXER_COMMAND_CXX_MANUAL:
		return "indexer command cxx manual";
	case INDEXER_COMMAND_CXX_CDB:
		return "indexer command cxx cdb";
	case INDEXER_COMMAND_JAVA:
		return "indexer command java";
	}
	return "indexer command unknown";
}
