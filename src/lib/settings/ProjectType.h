#ifndef PROJECT_TYPE_H
#define PROJECT_TYPE_H

#include <string>

enum ProjectType
{
	PROJECT_C_EMPTY,
	PROJECT_CPP_EMPTY,
	PROJECT_CXX_CDB,
	PROJECT_CXX_VS,
	PROJECT_JAVA_EMPTY,
	PROJECT_JAVA_MAVEN,
	PROJECT_UNKNOWN
};

std::string projectTypeToString(ProjectType v);
ProjectType stringToProjectType(std::string v);

#endif // PROJECT_TYPE_H
