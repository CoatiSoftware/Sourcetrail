#include "settings/ProjectType.h"

std::string projectTypeToString(ProjectType v)
{
	switch (v)
	{
	case PROJECT_C_EMPTY:
		return "Empty C Project";
	case PROJECT_CPP_EMPTY:
		return "Empty C++ Project";
	case PROJECT_CXX_CDB:
		return "C/C++ from Compilation Database";
	case PROJECT_CXX_VS:
		return "C/C++ from Visual Studio";
	case PROJECT_JAVA_EMPTY:
		return "Empty Java Project";
	case PROJECT_JAVA_MAVEN:
		return "Java Project from Maven";
	case PROJECT_UNKNOWN:
		break;
	}
	return "unknown";
}

ProjectType stringToProjectType(std::string v)
{
	if (v == projectTypeToString(PROJECT_C_EMPTY))
	{
		return PROJECT_C_EMPTY;
	}
	else if (v == projectTypeToString(PROJECT_CPP_EMPTY))
	{
		return PROJECT_CPP_EMPTY;
	}
	else if (v == projectTypeToString(PROJECT_CXX_CDB))
	{
		return PROJECT_CXX_CDB;
	}
	else if (v == projectTypeToString(PROJECT_CXX_VS))
	{
		return PROJECT_CXX_VS;
	}
	else if (v == projectTypeToString(PROJECT_JAVA_EMPTY))
	{
		return PROJECT_JAVA_EMPTY;
	}
	else if (v == projectTypeToString(PROJECT_JAVA_MAVEN))
	{
		return PROJECT_JAVA_MAVEN;
	}

	return PROJECT_UNKNOWN;
}
