#include "IndexerCommandType.h"

std::string indexerCommandTypeToString(IndexerCommandType type)
{
	switch(type)
	{
	case INDEXER_COMMAND_CXX:
		return "indexer_command_cxx";
	case INDEXER_COMMAND_JAVA:
		return "indexer_command_java";
	case INDEXER_COMMAND_PYTHON:
		return "indexer_command_python";
	case INDEXER_COMMAND_CUSTOM:
		return "indexer_command_custom";
	default:
		break;
	}
	return "indexer_command_unknown";
}

IndexerCommandType stringToIndexerCommandType(const std::string& s)
{
	if (s == indexerCommandTypeToString(INDEXER_COMMAND_CXX)) return INDEXER_COMMAND_CXX;
	if (s == indexerCommandTypeToString(INDEXER_COMMAND_JAVA)) return INDEXER_COMMAND_JAVA;
	if (s == indexerCommandTypeToString(INDEXER_COMMAND_PYTHON)) return INDEXER_COMMAND_PYTHON;
	if (s == indexerCommandTypeToString(INDEXER_COMMAND_CUSTOM)) return INDEXER_COMMAND_CUSTOM;
	return INDEXER_COMMAND_UNKNOWN;
}

