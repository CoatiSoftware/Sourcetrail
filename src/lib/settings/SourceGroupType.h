#ifndef SOURCE_GROUP_TYPE_H
#define SOURCE_GROUP_TYPE_H

#include <string>

enum SourceGroupType
{
	SOURCE_GROUP_C_EMPTY,
	SOURCE_GROUP_CPP_EMPTY,
	SOURCE_GROUP_CXX_CDB,
	SOURCE_GROUP_CXX_SONARGRAPH,
	SOURCE_GROUP_CXX_VS,
	SOURCE_GROUP_JAVA_EMPTY,
	SOURCE_GROUP_JAVA_MAVEN,
	SOURCE_GROUP_JAVA_GRADLE,
	SOURCE_GROUP_JAVA_SONARGRAPH,
	SOURCE_GROUP_UNKNOWN
};

std::string sourceGroupTypeToString(SourceGroupType v);
std::string sourceGroupTypeToProjectSetupString(SourceGroupType v);
SourceGroupType stringToSourceGroupType(const std::string& v);

#endif // SOURCE_GROUP_TYPE_H
