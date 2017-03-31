#include "settings/SourceGroupType.h"

std::string sourceGroupTypeToString(SourceGroupType v)
{
	switch (v)
	{
	case SOURCE_GROUP_C_EMPTY:
		return "C Source Group";
	case SOURCE_GROUP_CPP_EMPTY:
		return "C++ Source Group";
	case SOURCE_GROUP_CXX_CDB:
		return "C/C++ from Compilation Database";
	case SOURCE_GROUP_CXX_VS:
		return "C/C++ from Visual Studio";
	case SOURCE_GROUP_JAVA_EMPTY:
		return "Java Source Group";
	case SOURCE_GROUP_JAVA_MAVEN:
		return "Java Source Group from Maven";
	case SOURCE_GROUP_UNKNOWN:
		break;
	}
	return "unknown";
}

SourceGroupType stringToSourceGroupType(std::string v)
{
	if (v == sourceGroupTypeToString(SOURCE_GROUP_C_EMPTY))
	{
		return SOURCE_GROUP_C_EMPTY;
	}
	else if (v == sourceGroupTypeToString(SOURCE_GROUP_CPP_EMPTY))
	{
		return SOURCE_GROUP_CPP_EMPTY;
	}
	else if (v == sourceGroupTypeToString(SOURCE_GROUP_CXX_CDB))
	{
		return SOURCE_GROUP_CXX_CDB;
	}
	else if (v == sourceGroupTypeToString(SOURCE_GROUP_CXX_VS))
	{
		return SOURCE_GROUP_CXX_VS;
	}
	else if (v == sourceGroupTypeToString(SOURCE_GROUP_JAVA_EMPTY))
	{
		return SOURCE_GROUP_JAVA_EMPTY;
	}
	else if (v == sourceGroupTypeToString(SOURCE_GROUP_JAVA_MAVEN))
	{
		return SOURCE_GROUP_JAVA_MAVEN;
	}

	return SOURCE_GROUP_UNKNOWN;
}
