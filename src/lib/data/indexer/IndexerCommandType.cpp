#include "IndexerCommandType.h"

std::string indexerCommandTypeToString(IndexerCommandType type)
{
	switch(type)
	{
#if BUILD_CXX_LANGUAGE_PACKAGE
	case INDEXER_COMMAND_CXX:
		return "indexer_command_cxx";
#endif // BUILD_CXX_LANGUAGE_PACKAGE
#if BUILD_JAVA_LANGUAGE_PACKAGE
	case INDEXER_COMMAND_JAVA:
		return "indexer_command_java";
#endif // BUILD_JAVA_LANGUAGE_PACKAGE
#if BUILD_PYTHON_LANGUAGE_PACKAGE
	case INDEXER_COMMAND_PYTHON:
		return "indexer_command_python";
#endif // BUILD_PYTHON_LANGUAGE_PACKAGE
	case INDEXER_COMMAND_CUSTOM:
		return "indexer_command_custom";
	default:
		break;
	}
	return "indexer_command_unknown";
}

IndexerCommandType stringToIndexerCommandType(const std::string& s)
{
#if BUILD_CXX_LANGUAGE_PACKAGE
	if (s == indexerCommandTypeToString(INDEXER_COMMAND_CXX)) return INDEXER_COMMAND_CXX;
#endif // BUILD_CXX_LANGUAGE_PACKAGE
#if BUILD_JAVA_LANGUAGE_PACKAGE
	if (s == indexerCommandTypeToString(INDEXER_COMMAND_JAVA)) return INDEXER_COMMAND_JAVA;
#endif // BUILD_JAVA_LANGUAGE_PACKAGE
#if BUILD_PYTHON_LANGUAGE_PACKAGE
	if (s == indexerCommandTypeToString(INDEXER_COMMAND_PYTHON)) return INDEXER_COMMAND_PYTHON;
#endif // BUILD_PYTHON_LANGUAGE_PACKAGE
	if (s == indexerCommandTypeToString(INDEXER_COMMAND_CUSTOM)) return INDEXER_COMMAND_CUSTOM;
	return INDEXER_COMMAND_UNKNOWN;
}

